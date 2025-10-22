#ifndef __RPG_DATABASE_H__
#define __RPG_DATABASE_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_database.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

-----------------------------------------------------------------*/


#include <string>
#include <map>
#include "rpg_data_basic.h"

class RPG_Database
{
public:
	RPG_Database()		{}
	~RPG_Database()		{}

	void TEST_LoadHardcoded();
	void LoadXML( string xmlfile );

	/*------------------------------
		Convenience Access Tools
	------------------------------*/
	bool IsEquipment( string item_id );

public:
	std::map<string,wstring>				StringTable;
	std::map<string,RPG_ITEM>				Item;
	std::map<string,RPG_SPELL>				Spell;
	std::map<string,RPG_CLASS_DEFINITION>	Class;
	std::map<string,RPG_BAGTYPE>			Bagtype;
};

extern RPG_Database		*DATABASE;

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif //__RPG_DATABASE_H__