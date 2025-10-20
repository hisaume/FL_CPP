#ifndef __FL_FILE_H__
#define __FL_FILE_H__
/*---------------------------------------------------------------------------
|	|FILE|:    FL_File.h
|
|	FL Library
|	version 1.1
|----------------------------------------------------------------------------
	|NOTES|:

	Common file tasks. Generally, char-based is denoted with 'C' and
	wide-character version is denoted with 'W'.

---------------------------------------------------------------------------*/


// Suppress warning on deprecation for fopen etc.
//#pragma warning(disable:4996)


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include "FL_GeneralTools.h"

using namespace std;


//********		Namespace FL		********//
namespace FL
{


/*----------------------------------------------------------------------
|-|	FL_GetFileSize_

	Returns file size. char and wchar_t versions provided for filenames.
----------------------------------------------------------------------*/
inline long FL_GetFileSizeC( char *n )
{
	FILE *fp = fopen(n,"rb");
	if( fp==NULL ) return -1;	// error
	fseek( fp, 0, SEEK_END );
	long size = ftell(fp);	//rewind(fp);
	fclose(fp);
	return size;
};
inline long FL_GetFileSizeW( wchar_t *n )
{
	FILE *fp = _wfopen(n,L"rb");
	if( fp==NULL ) return -1;	// error
	fseek( fp, 0, SEEK_END );
	long size = ftell(fp);	//rewind(fp);
	fclose(fp);
	return size;
};

/*----------------------------------------------------------------------------
|-| FL_FileExists_

	Detects if the given file exists, via a template for char and wchar_t filenames.
----------------------------------------------------------------------------*/
template< class CHAR_TYPE >
inline bool FL_FileExists_( const CHAR_TYPE *filename )
{
	basic_ofstream< CHAR_TYPE > file( filename, ios::_Nocreate );
	if( file.fail() )
		return false;
	file.close();
	return true;
}
inline bool FL_FileExistsC( const char *filename )			{ return FL_FileExists_<char>( filename ); }
inline bool FL_FileExistsW( const wchar_t *filename )		{ return FL_FileExists_<wchar_t>( filename ); }
inline bool FL_FileExistsC( std::string &filename )			{ return FL_FileExistsC(filename.c_str()); }
inline bool FL_FileExistsW( std::wstring &filename )		{ return FL_FileExistsW(filename.c_str()); }

/*-------------------------------------------------------------
|-| FL_FileWriteString_

	WARNING:	For whatever reason, wide character version is not working in the template,
				so a separate implementation is used in FL_WriteFileStringW.

	Writes a text file from a string via template FL_FileWriteString_<>.
	It is typedef'd with char and wchar_t for convenience.
	e.g.

		bool result = flFileWriteStringC( "text_file.txt", "This text will be written." );
-------------------------------------------------------------*/
template< class CHAR_TYPE >
inline bool FL_FileWriteString_( const CHAR_TYPE *filename, const CHAR_TYPE *text, bool append =true )
{
	basic_ofstream< CHAR_TYPE > file;
	//IMBUE_NULL_CODECVT( file ) ;
	if( append )	file.open( filename, ios::out | ios::app );
	else			file.open( filename, ios::out );
	if( file.fail() ) return false;
	if( text ) {
		file << text;
		file.close();
	}
	return true;
}
// char version
inline bool FL_FileWriteStringC( const char *filename, const char *text, bool append =true )
{
	return FL_FileWriteString_< char >( filename, text, append );
}

// wchar_t stream io not working for the template, so here is the specific implementation.
inline bool FL_FileWriteStringW( const wchar_t *filename, const wchar_t *text, bool append =true )
{
	FILE *fp = NULL;
	if( append )	fp = _wfopen( filename, L"a+t,ccs=UTF-16LE" );
	else			fp = _wfopen( filename, L"wt,ccs=UTF-16LE" );
	if( !fp )	return false;
	fwrite( text, wcslen(text)*sizeof(wchar_t), 1, fp );
	fclose( fp );
	return true;
}
/*-------------------------------------------------------------
|-| FL_FileReadString_

	Reads a text file from a file name. char and wchar_t versions provided
	through a template flFileReadString_<>().
	e.g.

		string s = flFileReadStringC( "text_file.txt" );
-------------------------------------------------------------*/
template< class CHAR_TYPE >
inline basic_string< CHAR_TYPE > FL_FileReadString_( const CHAR_TYPE *filename )
{
	basic_ifstream< CHAR_TYPE > file( filename );
	if( file.fail() ) return basic_string< CHAR_TYPE>();
	basic_stringstream< CHAR_TYPE > buffer;
	buffer << file.rdbuf();
	file.close();
	return basic_string< CHAR_TYPE >( buffer.str() );
}

// char version.
inline string FL_FileReadStringC( const char *filename )			{ return FL_FileReadString_< char >( filename ); }
inline string FL_FileReadStringC( std::string &filename )			{ return FL_FileReadStringC(filename.c_str()); }

// wchar_t version. Specific implementation without template.
inline wstring FL_FileReadStringW( const wchar_t *filename )
{
	FILE *fp = NULL;
	fp = _wfopen( filename, L"rt,ccs=UTF-16LE" );
	if( !fp )	return wstring();
	//fseek( fp, 0, SEEK_END );
	//long fileSize = ftell( fp );
	//rewind( fp );
	wchar_t c=1;
	wstring txt;
	while( 1 ){
		c = getwc( fp );
		if( c == 65279 ) continue;	// binary ( 11111110 11111111 ) hex ( FEFF )
		if( c == WEOF ) break;
		else txt += c;
	}
	fclose( fp );
	return txt;
	//return FL_FileReadString_< wchar_t >( filename );
}
inline wstring FL_FileReadStringW( std::wstring &filename )			{ return FL_FileReadStringW(filename.c_str()); }
inline wstring FL_FileReadStringW( std::string &filename )			{ return FL_FileReadStringW( ASCII_to_UTF8(filename) ); }

/*-------------------------------------------------------------------------------
|-|
	Writing (dumping) and reading binary files. Good for storing data in a file.
	char and wchar (string/wstring) are internally supported.

	flBinaryWrite will create a new file if the given filename doesn't exist.

	e.g.
		struct TEST { int x, float y } z;
		// fill 'z' here. Then to dump it into a file:
		flBinaryWrite *f = new flBinaryWrite( "binary.dat" );
		f->Write( &z, sizeof(TEST) );

	Note that generic Write() takes a void pointer and the total size of stored data.
	Caller must allocate enough memory for the pointer to contain the data retrieved
	or it will cause bugs! Memory management is nonexistent within this class.

	To read the earlier binary.dat file:

		struct TEST z;	// ensure memory allocation is sufficient.
		filBinaryRead *f = new flBinaryRead( "binary.dat" );
		f->Read( &z, sizeof(TEST) );

	NOTE:	As a general rule, you should only use a fixed-size struct object
			when calling Read and Write.
			e.g.	passing a struct who has a pointer member char* which can be any size
					is a bad idea because the whole struct would have an arbitrary size.
			When using arbitrary size, Read and Write individual struct members
			rather than the whole struct in 1 go.

	Example: Dumping & Reading a fixed length struct.

				// Open file by making the object.
				flBinaryWrite *file = new flBinaryWrite( "binary.dat", true ); // overwrite 'true'
				if( file->IsOpen() )
					// then error while opening the file.
				struct S { int x, int y } s;	// fixed length struct
				s.x = 5; s.y = 12;
				file->Write( &s, sizeof(S) );
				delete file;					// close file.

		Then to read back from the file into struct s:

				flBinaryRead *file = new flBinaryRead( "binary.dat" );
				struct S s;
				file->Read( &s, sizeof(S) );	// read from file and fill out 's'.
				delete file;					// close file.

	Example: Dump & read strings with WriteStrRPG and ReadStrRPG methods.
			 Calling WriteStrRPG method automatically dumps string length in 32bit before dumping the string.

				// after opening a file as before...
				string s = "Hello World!";	// standard string.
				wstring w = L"�n���l";		// wide character version.
				file->WriteStrRPG( s );
				file->WriteStrRPG( w );
				// close file...

		Then to read back:

				string s;
				wstring w;
				file->ReadStrRPG( s );
				file->ReadStrRPG( w );

		ReadStrRPG() reads 32bit first for string length, then reads the actual string, as long as that length.

-------------------------------------------------------------------------------*/
class flBinaryWrite
{
public:
	flBinaryWrite( const char *n, bool overwrite=false )		{ if(overwrite) m_fp=fopen(n,"w+b"); else m_fp=fopen(n,"a+b"); }
	flBinaryWrite( const wchar_t *n, bool overwrite=false )		{ if(overwrite) m_fp=_wfopen(n,L"w+b"); else m_fp=_wfopen(n,L"a+b"); }
	virtual ~flBinaryWrite()									{ fclose(m_fp); }

	//----- Access methods -----
	bool	IsOpen()				{ if(m_fp) return true; return false; }
	// Standard call to write binary data to a file.
	int		Write( const void *data, int size_of )	{	return fwrite( data, size_of, 1, m_fp ); }
	//----- Helper methods using default Write() -----
	int		Write( const int *data )			{	return Write( data, sizeof(int) );		}
	int		Write( const char *data )			{	return Write( data, sizeof(char) );		}
	int		Write( const wchar_t *data )		{	return Write( data, sizeof(wchar_t) );	}
	int		Write( const float *data )			{	return Write( data, sizeof(float) );	}
	int		Write( const double *data )			{	return Write( data, sizeof(double) );	}
	int		Write( const string &s )			{	return Write( s.c_str(), sizeof(char)*s.length() );		}
	int		Write( const wstring &s )			{	return Write( s.c_str(), sizeof(wchar_t)*s.length() );	}
	// Specialized Write method to first dump 32bit value for string length, then dump the string itself.
	int		WriteStrRPG( const string &s )	{	int len=s.length(); Write( &len ); return Write( s );	}
	int		WriteStrRPG( const wstring &s )	{	int len=s.length(); Write( &len ); return Write( s );	}

protected:
	FILE *m_fp;
};

class flBinaryRead
{
public:
	flBinaryRead( const char *n )		{
		m_fp=fopen(n,"rb"); 
	}
	flBinaryRead( const wchar_t *n )	{ m_fp=_wfopen(n,L"rb"); }
	virtual ~flBinaryRead()				{ fclose(m_fp); }

	//----- Access methods -----
	bool	IsOpen()				{ if(m_fp==NULL) return false; return true; }
	// Standard call to read binary data to a file.
	int		Read( void *p, int size_of )			{	return fread( p, size_of, 1, m_fp ); }
	//----- Helper methods based on default Read().
	int		Read( string &s, int len )				{	char *c=new char[len]; c[len]=0;
														int res=Read(c,sizeof(char)*len); s=c; return res;
													}
	int		Read( wstring &s, int len )				{	wchar_t *c=new wchar_t[len]; c[len]=0;
														int res=Read(c,sizeof(wchar_t)*len); s=c; return res;
													}
	// Specialized method to read size (32bit long) first, then strings of that length.
	int		ReadStrRPG( string &s )					{	int len; Read(&len,sizeof(int)); return Read(s,len); }
	int		ReadStrRPG( wstring &s )				{	int len; Read(&len,sizeof(int)); return Read(s,len); }

public:
	FILE *m_fp;
};

/*-----------------------------------------------------------------------------
|-|	flTextFile

	Simple text output feature. It opens/creates a text file and writes a line.
	It always appends to the file so overwriting never happens.
	Every call to NewLine method opens then closes the file so it's (hopefully)
	crash safe, unless crash happens inside NewLine method.

	[USAGE]

	flTextFile text( "filename_here.txt" );
	text.NewLine( "whatever message." );
-----------------------------------------------------------------------------*/
class flTextFileW
{
	wstring	m_Filename;
public:
	flTextFileW()				{}
	flTextFileW( const wchar_t *n )	{ SetFilename( n ); }
	virtual ~flTextFileW()		{ m_Filename.clear(); }

	bool	FilenameSet()						{	if( m_Filename.length() ) return true; return false;	}
	void	SetFilename( const wchar_t *f )		{	m_Filename = f;	}

	// Writes a time stamp.
	bool	WriteTimeStamp( wstring append_txt =L"", bool _include_date =false, bool newline =false )
														{
															wstring msg;
															if( append_txt.length() > 0 )
																msg += append_txt;
															if( _include_date ) {
																msg += FL::FL_GetDate();	msg += L" ";
															}
															msg += FL::FL_GetTime();
															if(newline) msg += L"\n";
															return WriteText( msg.c_str() );
														}
	// Appends 1 line of text. Flag _include_time true if you want date/time stamp. _append_str is added after date stamp, if not NULL.
	bool	NewLine( const wchar_t *nl, const wchar_t* _append_str=NULL, bool _include_time=false ) 
										{
											if( !nl ) return false;	if( !FilenameSet() ) return false;
											wstring msg;
											if( _include_time )	{
												msg += FL::FL_GetDate();	msg += L" ";
												msg += FL::FL_GetTime();	msg += L" | ";
											}
											if( _append_str )		msg += _append_str;
											msg += nl;
											msg += L"\n";
											FILE *fp = _wfopen( m_Filename.c_str(), L"a+t,ccs=UTF-16LE" );
											if( !fp ) return false;
											fwrite( msg.c_str(), sizeof(wchar_t)*msg.length(), 1, fp );
											fclose( fp );
											return true;
										}
	// Writes an unformatted text. No newline is automatically added, so it's up to the caller to format the text.
	bool	WriteText( const wchar_t *txt )
										{
											if( !txt ) return false;	if( !FilenameSet() ) return false;
											wstring msg = txt;
											FILE *fp = _wfopen( m_Filename.c_str(), L"a+t,ccs=UTF-16LE" );
											if( !fp ) return false;
											fwrite( msg.c_str(), sizeof(wchar_t)*msg.length(), 1, fp );
											fclose( fp );
											return true;
										}
	bool	WriteText( const std::wstring txt )	{ return WriteText(txt.c_str()); }
};



}//********		End of Namespace FL		********//


/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif