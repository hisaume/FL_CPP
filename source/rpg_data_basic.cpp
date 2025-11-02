#include <rpg_data_basic.h>

/*------------------------------
------------------------------*/
void RPG_STATS::Clear( int fill )
{
	/*
		hp, maxhp;
		mp, maxmp;
		strength;		
		stamina;		
		physical;		
		accuracy_pct;	
		intelligence;	
	*/
	hp = maxhp = fill;
	mp = maxmp = fill;
	strength = fill;
	stamina = fill;
	physical = fill;
	accuracy_pct = fill;
	intelligence = fill;
}

void RPG_STATS::Copy( const RPG_STATS& from, int _exclusion_value )
{
	if( from.hp != _exclusion_value )
		hp = from.hp;
	if( from.maxhp != _exclusion_value )
		maxhp = from.maxhp;
	if( from.mp != _exclusion_value )
		mp = from.mp;
	if( from.maxmp != _exclusion_value )
		maxmp = from.maxmp;
	if( from.strength != _exclusion_value )
		strength = from.strength;
	if( from.stamina != _exclusion_value )
		stamina = from.stamina;
	if( from.physical != _exclusion_value )
		physical = from.physical;
	if( from.accuracy_pct != _exclusion_value )
		accuracy_pct = from.accuracy_pct;
	if( from.intelligence != _exclusion_value )
		intelligence = from.intelligence;	
}

/*------------------------------
	RPG_INVENTORYSLOT
------------------------------*/
RPG_INVENTORYSLOT* RPG_INVENTORY::GetSlot( int index )
{
	auto &iter = content.begin();	// a reference type. Object is NOT created!
	if( false == GetSlotIterator(index,iter) )
		return nullptr;
	return &(*iter);
}

bool RPG_INVENTORY::GetSlotIterator( int index, std::list<RPG_INVENTORYSLOT>::iterator &iterator )
{
	if( index >= max_size )
		return false;
	auto iter = content.begin();
	for( int i=0; i!=index; ++i )
		++iter;
	iterator = iter; // pass by reference.
	return true;
}

void RPG_INVENTORY::AddBagType( const string& bagtype_id, int size )
{
	bag_type.push_back( bagtype_id );
	max_size += size;
	content.resize( max_size );
	//int check_to_ensure_content_before_resize_still_exist_while_size_is_increased = 0;
}

int RPG_INVENTORY::AvailableSize()
{
	int vacant = 0;
	for( auto iter=content.begin(); iter!=content.end(); ++iter )
		if( iter->Empty() )
			++vacant;
	return vacant;
}

bool RPG_INVENTORY::Empty()
{
	if( max_size == AvailableSize() )
		return true;
	return false;
}

RPG_INVENTORYSLOT* RPG_INVENTORY::GetFirstEmptySlot()
{
	for( auto iter=content.begin(); iter!=content.end(); ++iter )
		if( iter->Empty() )
			return &(*iter);
	return nullptr;
}

/*-----------------------------

		RPG_SPELLBOOK

-----------------------------*/
bool RPG_SPELLBOOK::Learn( const string& spell_id, SPELLBOOK_ELEMENT *init )
{
	if( nullptr == GetState_Learned(spell_id) ) {
		if( init )
			learned_list[spell_id] = *init;
		else
			learned_list[spell_id] = SPELLBOOK_ELEMENT();
		return true;
	}
	return false;	// spell already exists.
}

bool RPG_SPELLBOOK::Unlearn( const string& spell_id )
{
	for( auto iter=learned_list.begin(); iter!=learned_list.end(); ++iter )
		if( iter->first == spell_id ) {
			learned_list.erase( iter );
			return true;
		}
	return false;
}

bool RPG_SPELLBOOK::Memorize( const string& spell_id )
{
	for( auto iter=learned_list.begin(); iter!=learned_list.end(); ++iter )
		if( iter->first == spell_id ) {
			SPELLBOOK_ELEMENT state = iter->second;
			learned_list.erase( iter );
			memorized_list[spell_id] = state;
			return true;
		}
	return false;
}

bool RPG_SPELLBOOK::Unmemorize( const string& spell_id )
{
	for( auto iter=memorized_list.begin(); iter!=memorized_list.end(); ++iter )
		if( iter->first == spell_id ) {
			SPELLBOOK_ELEMENT state = iter->second;
			memorized_list.erase( iter );
			learned_list[spell_id] = state;
			return true;
		}
	return false;
}

SPELLBOOK_ELEMENT* RPG_SPELLBOOK::GetState( const string& spell_id )							
{
	SPELLBOOK_ELEMENT *state;
	if( state=GetState_Memorized(spell_id) )
		return state;
	if( state=GetState_Learned(spell_id) )
		return state;
	return nullptr;
}

SPELLBOOK_ELEMENT* RPG_SPELLBOOK::GetState_Learned( const string& spell_id )
{
	for( auto iter=learned_list.begin(); iter!=learned_list.end(); ++iter )
		if( iter->first == spell_id )
			return &iter->second;
	return nullptr;
}

SPELLBOOK_ELEMENT* RPG_SPELLBOOK::GetState_Memorized( const string& spell_id )
{
	for( auto iter=memorized_list.begin(); iter!=memorized_list.end(); ++iter )
		if( iter->first == spell_id )
			return &iter->second;
	return nullptr;
}
