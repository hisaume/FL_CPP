#include <FL_General.h>
#include <FLO_Font.h>


void FLO_LoadCodePointsFromFile( FontPtr font, const std::wstring& codepoint_file )
{
	FILE *fp = _wfopen( codepoint_file.c_str(), L"rt,ccs=UTF-16LE" );
	if( !fp )	assert( 0 ); // file point is NULL.

	wchar_t c=1;	std::wstring txt;
	while( 1 ){
		c = getwc( fp );
		if( c == 65279 ) continue;	// binary ( 11111110 11111111 ) hex ( FEFF )
		if( c == WEOF ) break;
		else txt += c;
	}
	fclose( fp );

	// Now txt holds the entire string from the file. Set the codepoints.
	unsigned int i;
	for( i=0; i<txt.length(); ++i )
	{
		switch( txt[i] )
		{
		case L' ':
		case L'�@':
		case '\n':
		case '\t':
			continue;
		default:
			font->addCodePointRange( Ogre::Font::CodePointRange( txt[i], txt[i] ) );
		}
	}
}


/*------------------

	FL_OgreFont

------------------*/
FL_OgreFont::FL_OgreFont( const String& font_nickname, const String& ttf_filename,
							bool kana, KANJITYPE kanji_type, Real size, int resolution, const String& resource_group )
{
	LOG( L"Registering font " ); LOGVAR( ttf_filename ); LOG( L"... " );

	this->m_FontName = font_nickname;
	this->m_FontFilename = ttf_filename;
	this->m_KanaLoaded = kana;
	this->m_KanjiType = kanji_type;
	this->m_FontHeight = 0.04f;	// default

	FontManager &fontMgr = FontManager::getSingleton();
	m_FontHandle = fontMgr.create( m_FontName, resource_group );
	// Set properties
	m_FontHandle->setType( Ogre::FT_TRUETYPE );
	m_FontHandle->setSource( m_FontFilename );
	// size and resolution determine the quality of the loaded font
	m_FontHandle->setTrueTypeSize( size );
	m_FontHandle->setTrueTypeResolution( (unsigned int)resolution );
	// Set code point
	m_FontHandle->addCodePointRange( Ogre::Font::CodePointRange(33, 166) );	// default ASCII

	if( kana ) {
		m_FontHandle->setParameter("code_points", "12288-12543" );	// hiragana katakana
		m_FontHandle->setParameter("code_points", "65281-65439" );	// Fullwidth / halfwidth
	}

	switch( m_KanjiType )
	{
	case NONE:	// load none
		break;
	case J_SMALL:	// J (small pool)
		FLO_LoadCodePointsFromFile( m_FontHandle, CATDIR("CodePoints")+L"kanji_s.txt" );
		break;
	case J_LARGE:	// J (large pool)
		FLO_LoadCodePointsFromFile( m_FontHandle, CATDIR("CodePoints")+L"kanji_l.txt" );
		break;
	case C_LARGE:	// C pool
		m_FontHandle->setParameter("code_points", "19868-40868" );	// CJK Chinese
		break;
	default:
		break;
	}
	m_FontHandle->load();

	LOG( L" Done.\n" );
}


Real FL_OgreFont::FontWidth( Ogre::Font::CodePoint c ) const
{
	/*
		##### HACKED !! 0.74, 75 or 76 ####
		The extra multiplier used to get the correct value of ratio.
	*/
	Real ratio;
	switch( c )
	{
	case 32:		// ASCII space
	case 12288:		// CJK fullwidth space
		//ratio = m_FontHandle->getGlyphAspectRatio(L'A');
		//ratio *= 0.85f;
		//ratio *= 0.65f;
		ratio = 0.5f;
		ratio *= gDisplayRatio.ratio;		// 0.75f for 4:3, 0.56f for 16:9
		//if( gDisplayRatio.symbol==DR16X9 )
		//	ratio *= (3.0f/4.0f);	// REVIEW later
		break;
	default:
		/* Glyph ratio is in relation to 1:1 square ratio.
			As screen is likely 16:9 or 4:3, glyph ratio needs to be "shrunk" to retain
			correct perspective.
		*/
		ratio = m_FontHandle->getGlyphAspectRatio( c );
		ratio *= gDisplayRatio.ratio;		// 0.75f for 4:3, 0.56f for 16:9
		break;
	}
	return m_FontHeight * ratio;
}


Real FL_OgreFont::TextWidth( const DisplayString &txt ) const
{
	Real width = 0, char_width = 0;
	for( unsigned int i=0; i<txt.length(); ++i ) {
		char_width = FontWidth( txt[i] );
		width += char_width;
	}
	return width;
}


