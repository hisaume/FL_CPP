#ifndef __FLO_FONT_H__
#define __FLO_FONT_H__
/*-----------------------------------------------------------------
|	|FILE|:    FLO_Font.h
|
|	FL Library
|	version 2.11
|------------------------------------------------------------------
	|NOTES|:

	FL Ogre Font support and utilities.


	[ Directory Categories ]
		FL_OgreFont uses:	CATDIR("CodePoints")

		e.g.
			// Font codepoint
			gDirectoryManager.SetPath( "CodePoints", "/data/" );

	[ FL_OgreFont Example ]
		m_FontMain = new FL_OgreFont( "font_main", "M+1P+IPAG.ttf", true, J_SMALL, 18, 96 );
		m_FontMain->SetHeight( 0.04f );


-----------------------------------------------------------------*/
#include <Ogre.h>
#include <OgreFontManager.h>

#include <FL_GeneralTools.h>

using namespace Ogre;
using namespace FL;


enum KANJITYPE
{
	NONE, J_SMALL, J_LARGE, C_LARGE
};

/*
	Each object of this class represents "1 font".
	For each loaded font, height and colour can be changed afterwards.

	NOTE:
	Ogre::FontManager manages font internally, so this is really a wrapper and
	information container around what Ogre already provides.
*/
class FL_OgreFont
{
public:
	/* size: NOT final font size, but the font size FROM which this font is created.
		ttf_filename: MUST include path. */
	FL_OgreFont( String font_nickname, String ttf_filename,
					bool kana, KANJITYPE kanji_type, Real size=18, int resolution=96, String resource_group ="General" );

	String	GetName()							{ return m_FontName; }
	void SetHeight( Real font_height )			{ m_FontHeight=font_height; }
	/* Font height and width methods are in screen ratio. Full-length = 1.0f	*/
	Real GetHeight()							{ return m_FontHeight; }
	Real FontWidth( Ogre::Font::CodePoint c );
	Real TextWidth( const DisplayString &txt );

private:
	FontPtr m_FontHandle;		// font handle given by Ogre
	String	m_FontName;			// font nickname
	String	m_FontFilename;		// OS's TTF filename where font comes from
	Real	m_FontHeight;		// font height relative to screen i.e. final size of displayed font
	bool	m_KanaLoaded;
	/* Loading Kanji takes up memory. This allows different types of Kanji to be loaded.
		NONE:		no kanji loaded
		J_SMALL:	small pool of Japanese kanji
		J_LARGE:	large pool of Japanese kanji
		C_LARGE:	large pool of Chinese kanji		*/
	KANJITYPE m_KanjiType;	// Kanji type loaded for this font
};



/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	// __FLO_FONT_H__