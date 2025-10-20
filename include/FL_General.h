// FLRPG general definitions
#ifndef __FLRPG_H__
#define __FLRPG_H__
/*---------------------------------------------------------------------------
|	|FILE|:    FL_General.h
|
|	FL Library
|	version 2.4

---------------------------------------------------------------------------*/
// Comment this out for FL RPG game, but in for script editor EXE.
//#define FLRPG_SCRIPTCHECKER

// English version compiled with this def.
//#define FLRPG_ENGLISH

/*------------------------------------------------------------------------------------
						Message & Error output to Log file
------------------------------------------------------------------------------------*/
#include "FL_General.h"
#include "FL_File.h"

extern FL::flTextFileW						gLog;
//inline void RPGLOG(s)								{ gLog.WriteText( s );	}	// legacy
inline void LOG( std::wstring s )					{ gLog.WriteText( s ); }

inline void WARN( std::wstring s )					{ gLog.WriteText( std::wstring(L"*WARNING* ") + s ); }
inline void WARN( std::wstring v,std::wstring s )	{ gLog.WriteText( std::wstring(L"*WARNING* ") + std::wstring(L"[")+v+std::wstring(L"] ") + s ); }
inline void WARN( std::string v, std::wstring s )	{ WARN( FL::ASCII_to_UTF8(v), s ); }

inline void ERR( std::wstring s)					{ gLog.WriteText( std::wstring(L"*ERROR* ") + s ); }
inline void ERR( std::wstring v,std::wstring s )	{ gLog.WriteText( std::wstring(L"*ERROR* ") + std::wstring(L"[")+v+std::wstring(L"] ") + s ); }
inline void ERR( std::string v, std::wstring s )	{ ERR( FL::ASCII_to_UTF8(v), s ); }

inline void LOGVAR( std::wstring s, bool newline =false )	{	std::wstring m=L"["; m+=s; m+=L"]"; if(newline)m+=L"\n";
																gLog.WriteText(m.c_str());				}
inline void LOGVAR( std::string s, bool newline =false )	{ LOGVAR(FL::ASCII_to_UTF8(s), newline);	}
inline void LOGVAR( int i, bool newline =false )			{ LOGVAR(FL::LONG2WSTRING((long)i),newline);}
inline void LOGVAR( unsigned int i, bool newline =false )	{ LOGVAR(FL::LONG2WSTRING((long)i),newline);		}
inline void LOGVAR( float f, bool newline =false )			{ LOGVAR(FL::FLOAT2WSTRING(f));				}

inline void LOGHI( std::wstring s )							{ gLog.WriteText( s ); }
inline void LOGHIVAR( std::wstring s, bool newline =false )	{	std::wstring m=L"["; m+=s; m+=L"]"; if(newline)m+=L"\n";
																gLog.WriteText(m.c_str());				}
inline void LOGHIVAR( std::string s, bool newline =false )	{ LOGVAR(FL::ASCII_to_UTF8(s), newline);	}
inline void LOGHIVAR( int i, bool newline =false )			{ LOGVAR(FL::LONG2WSTRING((long)i),newline);}
inline void LOGHIVAR( unsigned int i, bool newline =false )	{ LOGVAR(FL::LONG2WSTRING((long)i),newline);		}
inline void LOGHIVAR( float f, bool newline =false )		{ LOGVAR(FL::FLOAT2WSTRING(f));				}


/*	gDirectoryManager is relied on by some FL library components.
	Application should declare this above main() and set appropriate category & paths.
	Macro CATDIR is provided for easy access.
	e.g. CATDIR("General") will return the path belonging to General category.
*/
extern FL::FL_DirectoryManager	gDirectoryManager;
#define	CATDIR(category)		gDirectoryManager.GetPath(category)
/*
	Display (monitor) ratio. Set this above the application main().
		e.g. DISPLAY_RATIO gDisplayRatio;
				gDisplayRatio.ratio = height / width;
				gDisplayRatio.symbol = DR4X3;
*/
extern FL::DISPLAY_RATIO		gDisplayRatio;
/*	String table for converting a GUID into a local language.
	Usually loaded thru FL_XML_StringTable.h.
	This must be defined in and around main() / WinMain().
*/
extern std::map<string,wstring>		gStringTable;
inline wstring STRING_TABLE( string st_id, wstring default_string )
{
	auto iter = gStringTable.begin();
	if( gStringTable.end() == (iter=gStringTable.find(st_id)) )
		return default_string;
	else
		return iter->second;
}
/*	User language
	& whether default language stored in main.fss is used.
*/
extern std::string		gUserLanguage;
extern bool				gUseDefaultLanguage;

/*----------------------------------
			Paths
----------------------------------*/
//namespace PATH
//{
//	const std::string XML = "./data/dbscan/";
//}// End namespace PATH


/*------------------------------------------------------------------------------------
	General game-wide definitions
------------------------------------------------------------------------------------*/


/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif