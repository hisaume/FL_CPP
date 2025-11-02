#include "rpg_script_flrpg.h"

#include <FL_General.h>
#include <FL_GeneralTools.h>


#ifndef FLRPG_SCRIPTEDITOR//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <rpg_globals.h>
#include <rpg_database.h>
#endif//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~



FL_ScriptFLRPG	*gScriptEngine = NULL;


/*-----------------------------------------------------------------------------------------------
	FLRPG Engine & Script Engine
-----------------------------------------------------------------------------------------------*/
#ifndef FLRPG_SCRIPTEDITOR//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~

//RPG_Engine			g_FLRPG;

void CreateScriptEngine()
{
	if( gScriptEngine )
		delete gScriptEngine;

	gScriptEngine = new FL_ScriptFLRPG;
	LOG( L"Script engine started.\n" );
	//g_FLRPG.go( g_ScriptEngine );
}
void DestroyScriptEngine()
{
	SAFE_DELETE( gScriptEngine );
	LOG( L"Script engine ended.\n" );
}
#endif//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~
/*-----------------------------------------------------------------------------------------------
	Script Globals
-----------------------------------------------------------------------------------------------*/
int		g_Script_SYSTEM = 0;
int		g_Script_ANSWER = 0;
string	g_Script_LANGUAGE;		// user chosen language. Script looks this up via 'LANGUAGE' variable.

void FL_ScriptSetLanguage( const string& lang )	{	g_Script_LANGUAGE = lang;	}

/*-----------------------------------------------------------------------------------------------
|-|	FL_ScriptFLRPG
-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
	DEBUG Engine functions : see FL_ScriptFLRPG::RegisterFunctions()
-----------------------------------------------------------------------*/
#define ENGINEDEBUGFUNC_SIZE	3
const char* SCRIPT_ENGINEDEBUG_FUNC[ENGINEDEBUGFUNC_SIZE] =
{
	"void DEBUG_Int( int )",
	"void DEBUG_String( string )",
	"void DEBUG_Print( string )"
};

#ifndef FLRPG_SCRIPTEDITOR//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~

void DEBUG_Int( int i )
{
	int _put_break_point_here = 0;
}
#include <rpg_state.h>
void DEBUG_String( std::string s )
{
	int _put_break_point_here = 0;
	STATE;
	std::wstring wTxt = FL::ASCII_to_UTF8( s );

	RPG_COMMAND com;
	com.verb = "DEBUG_BreakPoint"; //"DEBUG_String";
	com.subject = s;
	COMMAND.push_back( com );
}
void DEBUG_Print( std::string s )
{
	int _put_break_point_here = 0;
	std::wstring wTxt = FL::ASCII_to_UTF8( s );
}
/*------------------------------------------------------------------
	Engine functions : see FL_ScriptFLRPG::RegisterFunctions()
------------------------------------------------------------------*/
/*----------------------------------------------------------------------
	Engine functions
----------------------------------------------------------------------*/
void SCRIPT_DATABASE_RESERVE( std::string s )
{
	int SCRIPT_DATABASE_RESERVE_must_not_be_reached = 0;
	assert( SCRIPT_DATABASE_RESERVE_must_not_be_reached );
}

void SCRIPT_DATABASE_LoadXML( std::string s )
{
	RPG_COMMAND com;
	com.verb = "DATABASE_LoadXML";
	com.subject = s;
	COMMAND.push_back( com );
}

void SCRIPT_DATABASE_Item( std::string s )
{}
void SCRIPT_DATABASE_Equipment( std::string s )
{}

void SCRIPT_DATABASE_ClassTemplate( std::string s )
{
	//DATABASE->m_ResourceFilesCT.push_back( s );
}

void SCRIPT_DATABASE_Spell( std::string s )
{}
void SCRIPT_DATABASE_AddEffect( std::string s )
{}

//static DATA_Item *g_CurrentItemHandle;

void SCRIPT_DATABASE_AddItem( std::string name, std::string desc, int itemType, int effect, int stackSize, int useType,
							bool important, bool quest, bool alchemy, int equipPart, std::string meshName, int iconID )
{
}

void SCRIPT_DATABASE_EditItem(	int lvl, int exp,
								int atk, int def, int qck, int spd, int acc, int atkrange, int hpregen, int mpregen,
								int hp, int mp, int maxhp, int maxmp, int str, int agi, int dex, int wis, int intel,
								int mag, int fire, int ice, int wind, int poison,
								int esc, int punch, int kick, int critatk, int critmck, int counter,
								int doubleatk, int shield, int block, int dodge, int taunt, int concentrate,
								int stun, int sleep, int charm, int confusion, int dead )
{
}

void SCRIPT_DATABASE_AddClassTemplate( std::string s )
{
}
void SCRIPT_DATABASE_AddSpell( std::string s )
{
}

/*------------------------------------
			DEV Tools
------------------------------------*/
void SCRIPT_DEV_GiveAllEquipment()
{
	RPG_COMMAND com;
	com.verb = "DEV_GiveAllEquipment";
	COMMAND.push_back( com );
}

/*-----------------------------------
		General Scene segment
-----------------------------------*/

void SCRIPT_Title( std::string txt )
{
	std::wstring wTxt = FL::ASCII_to_UTF8( txt );
	g_Script_ANSWER = 0;
}
void SCRIPT_Description( std::string txt )
{
	std::wstring wTxt = FL::ASCII_to_UTF8( txt );
}
void SCRIPT_AddResourceFolder( std::string txt )
{
	std::wstring wTxt = FL::ASCII_to_UTF8( txt );
}
void SCRIPT_GotoZone( std::string scriptFile, int id )
{
	// open zone script file
	//g_FLRPG.GotoZone( std::string(SCRIPT_FILE_PATH)+scriptFile, id );
}
bool SCRIPT_CheckFlag( int flag_id )
{
	return false;
}
void SCRIPT_Position( int x, int z )
{
}
void SCRIPT_TerrainFile( std::string filename )
{
	// Set the XML DotScene filename
	//g_FLRPG.SCENE_Terrain( filename );
}
void SCRIPT_SetZoomPoint( int pinID )
{
}
void SCRIPT_ZoneDisplayName( std::string displayName )
{
	// REMINDER:	Zone GUID is the zone filename (fsz) minus extension.
	//				Display name is the name displayed to the player
}
/*-----------------------------------------------------------

							LIFE

-----------------------------------------------------------*/
void SCRIPT_LIFE_NewLife( string id, string job, string default_name, string default_desc, string which_zone )
{
	RPG_COMMAND com;
	com.verb = "LIFE_NewLife";
	com.s.push_back( id );				// 0
	com.s.push_back( job );				// 1
	com.s.push_back( default_name );	// 2
	com.s.push_back( default_desc );	// 3
	com.s.push_back( which_zone );		// 4
	COMMAND.push_back( com );
}

void SCRIPT_LIFE_DeleteLife( string id )
{
	RPG_COMMAND com;
	com.verb = "LIFE_GiveItem";
	com.s.push_back( id );
	COMMAND.push_back( com );
}

void SCRIPT_LIFE_GiveItem( string life_id, string item_id )
{
	RPG_COMMAND com;
	com.verb = "LIFE_GiveItem";
	com.s.push_back( life_id );
	com.s.push_back( item_id );
	COMMAND.push_back( com );
}

void SCRIPT_LIFE_GiveSpell( string life_id, string spell_id )
{
	RPG_COMMAND com;
	com.verb = "LIFE_GiveSpell";
	com.s.push_back( life_id );
	com.s.push_back( spell_id );
	COMMAND.push_back( com );
}

void SCRIPT_LIFE_Equip( string life_id, string item_id )
{
	RPG_COMMAND com;
	com.verb = "LIFE_Equip";
	com.s.push_back( life_id );
	com.s.push_back( item_id );
	COMMAND.push_back( com );
}

void SCRIPT_LIFE_GiveExp( string life_id, int exp )
{
	RPG_COMMAND com;
	com.verb = "LIFE_GiveExp";
	com.s.push_back( life_id );
	com.i.push_back( exp );
	COMMAND.push_back( com );
}

void SCRIPT_LIFE_DeleteItem( string life_id, string item_id )
{
	RPG_COMMAND com;
	com.verb = "LIFE_DeleteItem";
	com.s.push_back( life_id );
	com.s.push_back( item_id );
	COMMAND.push_back( com );
}

void SCRIPT_LIFE_AddBag( string life_id, string bag_type )
{
	RPG_COMMAND com;
	com.verb = "LIFE_AddBag";
	com.s.push_back( life_id );
	com.s.push_back( bag_type );
	COMMAND.push_back( com );
}
/*-----------------------------------------------------------

							WORLD

-----------------------------------------------------------*/
void SCRIPT_WORLD_OpenZone( string zone_id )
{
	RPG_COMMAND com;
	com.verb = "WORLD_OpenZone";
	com.s.push_back( zone_id );
	COMMAND.push_back( com );
}
/*-----------------------------------------------------------

							ZONE

-----------------------------------------------------------*/

/*-----------------------------------------------------------

							????

-----------------------------------------------------------*/
void SCRIPT_NewCharacter( int pinID, string passportName, int party, string displayName, string classID,
									string meshName, int level )
{
	//Vector3 v = g_FLRPG.GetPinPosition( pinID );
	//_prev_life = g_FLRPG.SCENE_NewCharacter( FL::ASCII_to_UTF8(displayName), classID,
	//											meshName, level, v.x, v.z, (1==party), passportName, true, v.y );
}

void SCRIPT_NewCharacterAtLoc( string passportName, int party, string displayName, string classID,
									string meshName, int level, float locX, float locZ )
{
	//_prev_life = g_FLRPG.SCENE_NewCharacter( FL::ASCII_to_UTF8(displayName), classID,
	//											meshName, level, locX, locZ, (1==party), passportName );
}

void SCRIPT_Talk( string header, string message, bool bYesNoTrigger )
{
	//g_FLRPG.GUI_OpenChatBox( FL::ASCII_to_UTF8(header), FL::ASCII_to_UTF8(message), bYesNoTrigger );
	//g_FLRPG.SuspendZoneEventScript();
}

void SCRIPT_Choice( int arraySize, CScriptArray &strArray )
{
	std::vector<wstring> wStr;
	wStr.reserve(arraySize);
	// Poll strings
	for( unsigned int i=0; i<strArray.GetSize(); ++i ) {
		wStr.push_back( FL::ASCII_to_UTF8(*((string*)strArray.At(i))) );
	}
	//g_FLRPG.GUI_OpenMatrixBox( arraySize, 1, wStr );
	//g_FLRPG.SuspendZoneEventScript();
}

void SCRIPT_SetAI( string life_id, bool ai_on )
{
	//g_FLRPG.SCENE_SetAI( life_id, ai_on );
}

void SCRIPT_ResetWaypoint( string guid )
{
	//g_FLRPG.SCENE_ResetWaypoint( LIFE_GUID(guid) );
}

void SCRIPT_SetWaypoint( string guid, int pinID )
{
	//g_FLRPG.SCENE_SetWaypoint( LIFE_GUID(guid), pinID );
}

void SCRIPT_ActionWalk( string guid )
{
	//g_FLRPG.SCENE_ActionWalk( LIFE_GUID(guid) );
}

#endif//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~
void SCRIPT_RESERVE()
{
	int SCRIPT_RESERVE_is_reached = 0;
	assert( SCRIPT_RESERVE_is_reached );
}

//------------------------------------------------
#define ENGINEFUNC_SIZE			200
const char* SCRIPT_ENGINE_FUNC[ENGINEFUNC_SIZE] =
{
	/*-------------------------------
				DATABASE
	-------------------------------*/
	"void DATABASE_LoadXML( string )",						// 0
	"void DATABASE_Item( string )",
	"void DATABASE_Equipment( string )",
	"void DATABASE_ClassTemplate( string )",
	"void DATABASE_Spell( string )",

	"void DEV_GiveAllEquipment()",							// 5
	"void DATABASE_RESERVE()",
	"void DATABASE_RESERVE()",
	"void DATABASE_RESERVE()",
	"void DATABASE_RESERVE()",

	"void DATABASE_AddEffect( string, int, int, int )",		// 10
	"void DATABASE_AddItem( string name, string desc, int itemType, int effect, int stackSize,	\
						int useType, bool important, bool quest, bool alchemy, int equipPart, string meshName, int iconID )",
	"void DATABASE_EditItem(	int lvl, int exp,																		\
								int atk, int def, int qck, int spd, int acc, int atkrange, int hpregen, int mpregen,	\
								int hp, int mp, int maxhp, int maxmp, int str, int agi, int dex, int wis, int intel,	\
								int mag, int fire, int ice, int wind, int poison,										\
								int esc, int punch, int kick, int critatk, int critmck, int counter,					\
								int doubleatk, int shield, int block, int dodge, int taunt, int concentrate,			\
								int stun, int sleep, int charm, int confusion, int dead )",
	"void DATABASE_AddClassTemplate( string, int, int )",
	"void DATABASE_AddSpell( string, int, int )",

	"void DATABASE_RESERVE()",								// 15
	"void DATABASE_RESERVE()",
	"void DATABASE_RESERVE()",
	"void DATABASE_RESERVE()",
	"void DATABASE_RESERVE()",

	"void Title( string )",									// 20
	"void Description( string )",
	"void AddResourceFolder( string )",
	"void GotoZone( string script_filename, int id )",
	"bool CheckFlag( int )",

	"void Position( int, int )",							// 25
	"void TerrainFile( string terrainFile )",
	"void ZoomPoint( int pinID )",
	"void ZoneDisplayName( string name )",
	"void RESERVE()",
	/*-------------------------------
				LIFE
	-------------------------------*/
	"void LIFE_NewLife( string id, string job, string def_name, string def_desc, string which_zone )",	// 30
	"void LIFE_DeleteLife( string id )",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void LIFE_GiveItem( string life_id, string item_id )",		// 35
	"void LIFE_GiveSpell( string life_id, string spell_id )",
	"void LIFE_Equip( string life_id, string item_id )",
	"void LIFE_GiveExp( string life_id, int exp )",
	"void RESERVE()",

	"void LIFE_DeleteItem( string life_id, string item_id )",	// 40
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 45
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void LIFE_AddBag( string life_id, string bag_type )",		// 50
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void NewCharacter( int pinID, string passportName, int party, string displayName, string classID, string meshName, int level )",		// 55
	"void NewCharacterAtLoc( string passportName, int bParty, string displayName, string classID, string meshName, int level, float locX, float locZ )",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 60
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void TalkGeneric( string header, string message, bool bYesNoTrigger )",			// 65
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void Choice( int arraySize, array<string>@ strArray )",		// 70
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 75
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 80
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	/*-------------------------------
				WORLD
	-------------------------------*/
	"void WORLD_OpenZone( string zone_id )",				// 85
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 90
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void ResetWaypoint( string GUID )",					// 95
	"void SetWaypoint( string GUID, int pinID )",
	"void ActionWalk( string GUID )",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 100
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 105
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 110
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 115
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	/*-------------------------------
				ZONE
	-------------------------------*/
	"void RESERVE()",										// 120
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 125
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 130
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 135
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 140
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 145
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 150
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 155
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 160
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 165
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 170
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 175
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 180
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 185
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 190
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",

	"void RESERVE()",										// 195
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()",
	"void RESERVE()"
};

#ifndef FLRPG_SCRIPTEDITOR//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~
void FL_ScriptFLRPG::RegisterFunctions()
{
	int r;
	/*---------------------------------------
				Debug Functions
	---------------------------------------*/
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINEDEBUG_FUNC[0], asFUNCTION(DEBUG_Int), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINEDEBUG_FUNC[1], asFUNCTION(DEBUG_String), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINEDEBUG_FUNC[2], asFUNCTION(DEBUG_Print), asCALL_CDECL);	assert( r >= 0 );

	/*---------------------------------------
				Engine Functions
	---------------------------------------*/
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[0], asFUNCTION(SCRIPT_DATABASE_LoadXML), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[1], asFUNCTION(SCRIPT_DATABASE_Item), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[2], asFUNCTION(SCRIPT_DATABASE_Equipment), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[3], asFUNCTION(SCRIPT_DATABASE_ClassTemplate), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[4], asFUNCTION(SCRIPT_DATABASE_Spell), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[5], asFUNCTION(SCRIPT_DEV_GiveAllEquipment), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[6], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[7], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[8], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[9], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[10], asFUNCTION(SCRIPT_DATABASE_AddEffect), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[11], asFUNCTION(SCRIPT_DATABASE_AddItem), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[12], asFUNCTION(SCRIPT_DATABASE_EditItem), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[13], asFUNCTION(SCRIPT_DATABASE_AddClassTemplate), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[14], asFUNCTION(SCRIPT_DATABASE_AddSpell), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[15], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[16], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[17], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[18], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[19], asFUNCTION(SCRIPT_DATABASE_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[20], asFUNCTION(SCRIPT_Title), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[21], asFUNCTION(SCRIPT_Description), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[22], asFUNCTION(SCRIPT_AddResourceFolder), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[23], asFUNCTION(SCRIPT_GotoZone), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[24], asFUNCTION(SCRIPT_CheckFlag), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[25], asFUNCTION(SCRIPT_Position), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[26], asFUNCTION(SCRIPT_TerrainFile), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[27], asFUNCTION(SCRIPT_SetZoomPoint), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[28], asFUNCTION(SCRIPT_ZoneDisplayName), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[29], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	//------	LIFE	------
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[30], asFUNCTION(SCRIPT_LIFE_NewLife), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[31], asFUNCTION(SCRIPT_LIFE_DeleteLife), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[32], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[33], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[34], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[35], asFUNCTION(SCRIPT_LIFE_GiveItem), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[36], asFUNCTION(SCRIPT_LIFE_GiveSpell), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[37], asFUNCTION(SCRIPT_LIFE_Equip), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[38], asFUNCTION(SCRIPT_LIFE_GiveExp), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[39], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[40], asFUNCTION(SCRIPT_LIFE_DeleteItem), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[41], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[42], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[43], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[44], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[45], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[46], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[47], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[48], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[49], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[50], asFUNCTION(SCRIPT_LIFE_AddBag), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[51], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[52], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[53], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[54], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[55], asFUNCTION(SCRIPT_NewCharacter), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[56], asFUNCTION(SCRIPT_NewCharacterAtLoc), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[57], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[58], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[59], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[60], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[61], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[62], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[63], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[64], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[65], asFUNCTION(SCRIPT_Talk), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[66], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[67], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[68], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[69], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[70], asFUNCTION(SCRIPT_Choice), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[71], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[72], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[73], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[74], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[75], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[76], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[77], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[78], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[79], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[80], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[81], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[82], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[83], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[84], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	//------	WORLD	------
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[85], asFUNCTION(SCRIPT_WORLD_OpenZone), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[86], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[87], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[88], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[89], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[90], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[91], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[92], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[93], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[94], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[95], asFUNCTION(SCRIPT_ResetWaypoint), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[96], asFUNCTION(SCRIPT_SetWaypoint), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[97], asFUNCTION(SCRIPT_ActionWalk), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[98], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[99], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[100], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[101], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[102], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[103], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[104], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[105], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[106], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[107], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[108], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[109], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[110], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[111], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[112], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[113], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[114], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[115], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[116], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[117], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[118], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[119], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	//------	ZONE	------
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[120], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[121], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[122], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[123], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[124], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[125], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[126], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[127], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[128], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[129], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[130], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[131], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[132], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[133], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[134], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[135], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[136], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[137], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[138], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[139], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[140], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[141], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[142], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[143], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[144], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[145], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[146], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[147], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[148], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[149], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[150], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[151], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[152], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[153], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[154], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[155], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[156], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[157], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[158], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[159], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[160], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[161], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[162], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[163], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[164], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[165], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[166], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[167], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[168], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[169], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[170], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[171], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[172], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[173], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[174], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[175], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[176], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[177], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[178], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[179], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[180], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[181], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[182], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[183], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[184], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[185], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[186], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[187], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[188], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[189], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[190], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[191], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[192], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[193], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[194], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[195], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[196], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[197], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[198], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[199], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );

	/*---------------------------------------
				Global Properties
	---------------------------------------*/
	r = m_ScriptEngine->RegisterGlobalProperty("int SYSTEM", &g_Script_SYSTEM); assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalProperty("int ANSWER", &g_Script_ANSWER); assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalProperty("string LANGUAGE", &g_Script_LANGUAGE); assert( r >= 0 );
}
#else//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~
void FL_ScriptFLRPG::RegisterFunctions()
{
	int r;
	//----- Engine Debug functions -----
	for( int i=0; i<ENGINEDEBUGFUNC_SIZE; ++i ) {
		r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINEDEBUG_FUNC[i], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	}
	//----- Engine functions -----
	for( int i=0; i<ENGINEFUNC_SIZE; ++i ) {
		r = m_ScriptEngine->RegisterGlobalFunction(SCRIPT_ENGINE_FUNC[i], asFUNCTION(SCRIPT_RESERVE), asCALL_CDECL);	assert( r >= 0 );
	}
	//----- Register global properties -----
	r = m_ScriptEngine->RegisterGlobalProperty("int SYSTEM", &g_Script_SYSTEM); assert( r >= 0 );
	r = m_ScriptEngine->RegisterGlobalProperty("int ANSWER", &g_Script_ANSWER); assert( r >= 0 );
}
#endif//~~~~~~~~~~~~~~~~~~~~~~~~~~ For Script Editor ~~~~~~~~~~~~~~~~~~~~~~~~~~
void FL_ScriptFLRPG::RegisterMethods()	{}

/*-----------------------------------------------------------------------------------------------

	Script Callback functions :	Must be given in each main script file
								by script writer.

			CALLBACK_InitializeResources()
			CALLBACK_Main( ... )

-----------------------------------------------------------------------------------------------*/
#define STORYFUNC_SIZE		4
const char* SCRIPT_STORY_FUNC[STORYFUNC_SIZE] =
{
	"void CALLBACK_InitializeResources()",						// 0
	"void CALLBACK_Main( string, string, string, string )"		// 1
};

int FL_ScriptFLRPG::RUN_CALLBACK_InitializeResources()
{
	return RunSCRIPT( SCRIPT_STORY_FUNC[0] );
}

int FL_ScriptFLRPG::RUN_CALLBACK_Main( const string& s1, const string& s2, const string& s3, const string& s4 )
{
	string str_stack[4];
	str_stack[0] = s1;
	str_stack[1] = s2;
	str_stack[2] = s3;
	str_stack[3] = s4;
	return RunSCRIPT_StringStringStringString( SCRIPT_STORY_FUNC[1], str_stack );
}

/*----------------------------------------------------------------
	This function checks to ensure all the conpulsary callback
	functions in the main script are provided by the
	script writer.
----------------------------------------------------------------*/
bool FL_ScriptFLRPG::CheckScriptCallbacks( SCRIPTTYPE type )
{
	std::string msg;
	bool error = false;
	int i;

	if( type == SCRIPTTYPE_STORY )
	{
		for( i=0; i<STORYFUNC_SIZE; ++i )
		{
			if( GetFunctionID(SCRIPT_STORY_FUNC[i]) < 0 ) {
				error = true;
				msg = std::string("Callback error: Story script must have ")+SCRIPT_STORY_FUNC[i]; msg+="\n";
				m_ScriptMessage.push_back( msg );
#ifdef SCRIPT_PRINTF
				printf( "%s\n", msg.c_str() );
#endif
			}
		}
	}
	else if( type == SCRIPTTYPE_ZONE )
	{
		/*
		for( i=0; i<ZONEFUNC_SIZE; ++i )
		{
			if( GetFunctionID(SCRIPT_ZONE_FUNC[i]) < 0 ) {
				error = true;
				msg = std::string("Callback error: Zone script must have ")+SCRIPT_ZONE_FUNC[i]; msg+="\n";
				m_ScriptMessage.push_back( msg );
#ifdef SCRIPT_PRINTF
				printf( "%s\n", msg.c_str() );
#endif
			}
		}*/
	}
	return !error;
}
