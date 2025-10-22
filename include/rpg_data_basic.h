#ifndef	__RPG_DATA_BASIC_H__
#define __RPG_DATA_BASIC_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_data_basic.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

-----------------------------------------------------------------*/
#include <string>
#include <deque>
#include <list>
#include <FL_GeneralTools.h>


using namespace FL;


/*----------------------------------------
		Data Objects - base objects
----------------------------------------*/
struct RPG_DEFENCE
{
	int		physical;
	int		neutral;
	int		fire;
	int		water;
	int		poison;

	RPG_DEFENCE()			{ Clear(); }
	void Clear()			{ physical = neutral = fire = water = poison = 0; }
};

struct RPG_STATS
{
	int		hp, maxhp;
	int		mp, maxmp;
	int		strength;		// ちから		atk = strength + weapon
	int		stamina;		// たいりょく	hp = stamina * class multiplier?
	//int		physical;		// みのまもり	def = physical + armour
	int		agility;		// Agility/dodge
	int		dexterity;		// きようさ		melee accuracy
	int		intelligence;	// かしこさ		spell strength, maxmp = intel * class multiplier?

	RPG_STATS()						{ Clear(); }
	void Clear( int fill =0 );
	// Copies elements from 'from', except for elements which carry the _exclusion_value value.
	void Copy( const RPG_STATS& from, int _exclusion_value =-999999 );
};

struct RPG_STATS_ANOMALY
{
	int		sleep;
	int		stun;		// momental sleep, non persistent. Not stackable.
};

/*	Describes all effects from item effects to DOT effect.
	NOTE:	stun and sleep (anomaly) should be directly described in 'actual_effect'.

	'actual effect' :=
		"insta_damage_physical"
		"insta_damage_magic"
		"insta_damage_fire"
		"insta_damage_water"
		"insta_heal_hp"
		"insta_heal_mp"
		"insta_heal_poison"
*/
struct RPG_EFFECT
{
	enum EFFECT_AREA {
		SINGLE, ALL
	} effect_area;					// single target or all target

	string		process;			// describes the actual processing effect e.g. "poison", "fire", "heal_hp", "none"
	string		elemental;			// elemental alignment of the process. Used mainly to do a resistance check.
	bool		overtime;			// false for insta effect, true for overtime. false by default.
	int			value;
	bool		offensive_effect;	// true if undesirable effect like causing a damaage. false for healing etc.
	float		duration;			// In secs. How long the stick_effect stays on. Valid only with certain effect_id. always +ve.

	RPG_STATS		stick_stats;	// overtime
	RPG_DEFENCE		stick_defence;	// overtime
};

struct RPG_EQUIPSLOTS
{
	std::deque<string>		slot;			// holds equipment ID
	RPG_EQUIPSLOTS() : slot(SLOT_SIZE)	{}

	enum EQUIPSLOT {
		RHAND =0,	// "rhand"
		LHAND,		// "lhand"
		HEAD,		// "head"
		BODY,		// "body"
		FINGER,		// "finger"
		SLOT_SIZE,	//---- Note: above SLOT_SIZE it is the actual slots! Below it's conceptual.
		TWOHAND,		// Two Handed (occupy both hands)
		EITHERHAND,		// Either Hand
		SLOT_SIZE2
	};
};
const string EQUIPSLOT_ID[RPG_EQUIPSLOTS::SLOT_SIZE2] = {	// Make sure this order corresponds to enum EQUIPSLOT above.
	"equipslot_rhand",
	"equipslot_lhand",
	"equipslot_head",
	"equipslot_body",
	"equipslot_finger",
	"equipslot_",
	"equipslot_twohand",
	"equipslot_eitherhand"
};	// The above are the IDs for string look-up-table, multi language support.

struct RPG_INVENTORYSLOT
{
	string		guid;
	int			quantity;

	RPG_INVENTORYSLOT() : quantity(0)	{}
	/* Convenience method to check quantity against 0. @Returns true if empty. */
	bool Empty()										{ return quantity==0?true:false; }
	void Clear()										{ guid.clear(); quantity=0; }
	/* Pass reference into this method to swap content.
		On return, the passed object will contain what was in this object. */
	void SwapContent( RPG_INVENTORYSLOT &with_this )	{ RPG_INVENTORYSLOT s=with_this; with_this=*this; *this=s; }
};

struct RPG_INVENTORY
{
	std::list<RPG_INVENTORYSLOT>	content;
	int								max_size;
	std::deque<string>				bag_type;		// e.g. 小さなふくろ but of course thru string table.

	RPG_INVENTORY() : max_size(0)	{}
	/* @RETURN the handle of the requested slot. NULL if index is out of bounds. */
	RPG_INVENTORYSLOT* GetSlot( int index );
	/* Pass a reference type iterator as 2nd parameter i.e. std::list<RPG_INVENTORYSLOT>::iterator &iter.
		@RETURN false if index is out of bounds and hence the reference iterator returns unusable. */
	bool GetSlotIterator( int index, std::list<RPG_INVENTORYSLOT>::iterator &iterator );
	/* Convenience method to increase content size. */
	void AddBagType( string bagtype_id, int size );
	/* @RETURN true if bagtype_id already exists in the bag_type stack. */
	bool BagTypeExists( string bagtype_id )		{ for(auto i=bag_type.begin();i!=bag_type.end();++i) if(*i==bagtype_id) return true; return false; }
	/* @RETURN the number of vacant slots. */
	int AvailableSize();
	/* @RETURN true if inventory is empty. */
	bool Empty();
	/* @RETURN nullptr if there is no room left. */
	RPG_INVENTORYSLOT* GetFirstEmptySlot();
};

/*	Property sheet of 1 Weapon or armor (shield)
*/
#define	STR_NEUTRAL	"neutral"
#define	STR_FIRE	"fire"
#define	STR_WATER	"water"
#define	STR_POISON	"poison"

struct RPG_EQUIPMENT_PROPERTY
{
	int						atk_physical;	// attack points, physical damage.
	int						atk_elemental;	// attack points, elemental damage.
	string					element;		// elemental alignment of atk_elemental e.g. "neutral", "fire", "water"
	RPG_STATS				stats;			// stats gotten thru equipping it.
	RPG_DEFENCE				def;			// defence points of an armour.
	RPG_EQUIPSLOTS::EQUIPSLOT	slot;			// where exactly to equip it.
	// wear_category_robe, wear_category_marmour etc.
	string						wear_category;	// e.g. ローブ系, 重鎧系, Light, Medium, Heavy etc. Must be matched by character/class ability to wear.
	int							wear_category_level;
};

/*	Describes 1 Ability, which can include a UI command.
*/
struct RPG_ABILITY
{
	enum ABILITY_TYPE {
		COMMAND, ADD, MULTIPLY, SPECIAL
	} type;
	string		id;				// ability GUID
	string		ui_command;		// string ID for battle command entry.
	string		special;		// process command (actual effect).
	int			level;
	int			experience;
	int			iParam;
	float		fParam;

	RPG_ABILITY() : level(0), experience(0), iParam(0), fParam(0)	{}
};

/*------------------------------------------

		Data Objects - final objects

------------------------------------------*/
struct RPG_ABILITYBOOK
{
	std::map<string,RPG_ABILITY>			list;

	RPG_ABILITY* GetUICommand( string ui_com );
	void GetUICommandList( std::deque<string>& ui_com_list );
	//int GetTotalStatsModifier( string stats_element );	*** Put these in Helper? **
	//float GetTotalStatsMultiplier( string stats_element );
};

struct RPG_SPELL
{
	wstring				default_name, default_desc;		// strings in default language.

	string				guid;					// spell name
	RPG_EFFECT			spell_effect;
	FL_Timer			cast_timer;				// casting duration & timer
	FL_Timer			refresh_timer;			// re-cast duration & timer
};

struct RPG_ITEM
{
	enum TYPE {			// Broad categorization of an item. Mostly for sorting.
		UNKNOWN =0,
		ITEM,
		WEAPON,
		ARMOUR,
		ACCESSORY,
		TYPE_SIZE
	} item_type;

	enum USABILITY {	// use as item
		NOUSE,
		ONCE,
		UNLIMITED
	} usability;

	wstring		default_name, default_desc;		// strings in default language

	string		guid;			// item name
	RPG_EFFECT	use_effect;
	int			stack_size;		// stackable size in inventory.
	bool		quest_item;		// true if can't be sold or droped.
	int			price;

	RPG_EQUIPMENT_PROPERTY	equipment;	// item as equipment
};

/*
	Learn = acquire the spell in the 1st place.
	Memorize = ready an already learned spell for real use.

	Memorize moves a spell from learned_list to memorized_list.
	Unmemorize reverses the process.
*/
struct SPELLBOOK_ELEMENT {
	// change this content as necessary.
	int level;
	float multiplier;

	SPELLBOOK_ELEMENT() : level(1), multiplier(1.0f) {}
};
struct RPG_SPELLBOOK
{
	std::map<string,SPELLBOOK_ELEMENT>	learned_list;		// e.g. spell proficiency = spell_list["spell_mera"]
	std::map<string,SPELLBOOK_ELEMENT>	memorized_list;		// e.g. spell proficiency = spell_list["spell_mera"]

	bool Learn( string spell_id, SPELLBOOK_ELEMENT *init =nullptr );
	bool Unlearn( string spell_id );
	/* Moves a spell from learned list to memorized list. @RETURN false if spell not found in learned_list. */
	bool Memorize( string spell_id );
	bool Unmemorize( string spell_id );
	/* Searches both lists. @RETURNS nullptr if spell is not present. */
	SPELLBOOK_ELEMENT* GetState( string spell_id );
	/* Searches learned list. @RETURN nullptr if spell is not present. */
	SPELLBOOK_ELEMENT* GetState_Learned( string spell_id );
	/* Searches memorized list. @RETURN nullptr if spell is not present. */
	SPELLBOOK_ELEMENT* GetState_Memorized( string spell_id );
};
/*---------------------------------------------------
	Class Definition (i.e. Template)

	Defines:	necessary exp for new lvl
				stats upgrade per lvl
				(?) skill/spell obtained per lvl
---------------------------------------------------*/
#define RPG_MAXLEVEL		100		// actual max level+1
struct RPG_CLASS_DEFINITION
{
	wstring		default_name, default_desc;		// strings in default language

	string		guid;							// class guid. NOT instance guid of a particular character!!
	int			base_level;						// default starting level
	RPG_STATS	stats[RPG_MAXLEVEL];			// actual full stats for each level.
	int			exp2next_lvl[RPG_MAXLEVEL];		// exp needed AT bracketed level e.g. exp needed to level 2 = exp_next_lvl[1]
	/*	Default categories of equipment this class can equip e.g. wear_category["LIGHT_ARMOR"] = 0 if he can NOT wear it.
		Integer value from 1 indicates the default level of proficiency for the given category (usually 1 by default).
		Must match RPG_EQUIPMENT_PROPERTY::wear_category to wear that equipment.
	*/
	std::map<string, int>		wear_category;

	RPG_CLASS_DEFINITION()						{	Clear(); }
	void Clear()								{	guid.clear(); base_level=1; for(int i=0;i<RPG_MAXLEVEL;++i){ exp2next_lvl[i]=0; stats[i].Clear(1); } }
};

/*---------------------------------------------------
	Bag Type

	Simple struct to hold:
		string ID, size, default display name & desc.
---------------------------------------------------*/
struct RPG_BAGTYPE
{
	string	bagtype_id;
	int		size;
	wstring	default_name, default_desc;

	RPG_BAGTYPE() : size(0)	{}
};

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__RPG_DATA_BASIC_H__