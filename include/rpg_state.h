#ifndef __RPG_STATE_H__
#define __RPG_STATE_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_state.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

-----------------------------------------------------------------*/
#include <rpg_globals.h>
#include <rpg_data_basic.h>



/*
*/
struct AGGRO_TARGET
{
	string	target_id;
	int		value;
};

struct AGGRO_LIST
{
	std::list<AGGRO_TARGET>		alist;

	void Clear();
	void Add( string life_id, int value );
	void Subtract( string life_id, int value );
	void PushToTop( string life_id );
	void Remove( string life_id );
	string GetNth( int rank );
	string GetFirst();
};

/*
	RPG_EFFECT from database needs this wrapper to be instantiated.
*/
struct EFFECT_STATE
{
	enum SOURCE_TYPE {
		ITEM, SPELL, ABILITY
	} source_type;

	string		by_whom;		// Who used/casted this into existence.
	string		source_name;	// GUID of the item/spell/ability which triggered this effect.
	FL_Timer	timer;			// duration timer.

	RPG_EFFECT	data;
};

/*-------------------------------------------

				Life State

	A life instance is a combination of:
		- 'PNP' component, which is Pen & Paper component, such as stats, equipment, classID etc
		- 'AVATAR' component, such as currently executing action & its phases, position etc. Physical aspects.

-------------------------------------------*/
struct LIFE_PNP		// life data in pen&paper RPG fashion.
{
	wstring					default_name;
	wstring					default_desc;

	string						guid;			// given by script, OR class_id+factory_id if automatically spawned (in string).
	string						class_id;		// which class he belongs to.
	long						factory_id;		// Used only if automatically spawned.
	int							level;
	int							experience;
	RPG_STATS					stats;
	RPG_EQUIPSLOTS				equipment;
	RPG_INVENTORY				inventory;
	RPG_SPELLBOOK				spellbook;		// list of spells.
	RPG_ABILITYBOOK				abilitybook;
	std::deque<string>			commandlist;
	/*	Categories of equipment this life can equip e.g. wear_category["LIGHT_ARMOR"] = 0 if he can NOT wear it. Default set should be given
		by a class template from which this object is created. Integer value from 1 is the proficiency level for the given category.
		Must match RPG_EQUIPMENT_PROPERTY::wear_category to wear that equipment.	*/
	std::map<string, int>		wear_category;

	void GenerateGUID()		{ guid = class_id + FL::LONG2STRING(factory_id); }
	/* Convenience method for wear_category member.
		@return 0 if the given category is not wearable, 1 or above indicates the level of proficiency. */
	int GetWearProficiency( string category )	{	for( auto iter=wear_category.begin(); iter!=wear_category.end(); ++iter ) if( iter->first == category ) return iter->second; return 0; }
};

struct LIFE_ACTION	// action messages
{
	std::deque<string>	waiting_list;		// queue of future commands if there are any.
	string				current_state;		// current action state. Melee, Meditating, Chanting/Casting.
	RPG_ACTIONPHASE		phase_tracker;		// phase tracking for the current state above.

	int			action_i[5];		// params in integer
	float		action_f[5];		// params in float
};

struct LIFE_EFFECT
{
	std::list<EFFECT_STATE>	normal_stack;		// buffs, debuffs, DoT, etc.

	string		stun_origin;
	string		sleep_origin;
	FL_Timer	stun;			// +ve time left if stunned.
	FL_Timer	sleep;			// +ve time left if asleep.

	bool Clear();
	bool IsStunned();
	bool IsAsleep();
	void WakeUp();
};

/*	pnp & avatar compound describing a life object.
*/
struct LIFE_OBJECT
{
	LIFE_PNP		pnp;		// pen & paper data
	LIFE_ACTION		act;		// action state
	LIFE_EFFECT		fx;			// effect stack

	string		motion_state;
	string		zone_id;
	float		x, z;
	//struct V3

	// ****** WHAT ABOUT APPEARANCE ?? SKIN ?? ***************
};

/*-------------------------------------------

			Data for 1 Zone.

-------------------------------------------*/
struct RPG_ZONE
{
	string							zone_id;
	std::map<string,LIFE_OBJECT>	lifelist;
};

/*-------------------------------------------

				World State

-------------------------------------------*/
struct RPG_State
{
	string							current_zone;
	std::map<string,RPG_ZONE>		zonelist;
	std::map<string,LIFE_OBJECT>	partylist;
	RPG_INVENTORY					inventory_item;
	RPG_INVENTORY					inventory_equip;
	RPG_INVENTORY					bank_item;			// bank vault
	RPG_INVENTORY					bank_equip;

	RPG_State()			{}
	/* Specialized version of GetZone(). @RETURN the current zone handle.
		@REMARK This method should always return a valid handle because a game can't exist
				without a zone i.e. there should be a current zone. */
	inline RPG_ZONE*	CurrentZone();
	RPG_ZONE*	GetZone( string zone_id );
	/* Like GetLife( guid ), but also returns whether he's a party member or not in 'is_party'.
		'is_party' is strictly a return value! It does not change the behaviour of this method. */
	LIFE_OBJECT*		GetLife( string guid, bool& is_party );
	/* @RETURN LIFE_OBJECT handle to the given life. nullptr is returned if not found.
		@REMARK Order of search: Party member list, local zone list, the other zones. */
	LIFE_OBJECT*		GetLife( string guid )					{ bool b; return GetLife(guid,b); }
	/* Removes the given life. @RETURN true if successful, false if not found.
		@REMARK Order of search: Party member list, local zone list, the other zones. */
	bool				DeleteLife( string guid );
};

extern RPG_State	*STATE;
/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__RPG_STATE_H__