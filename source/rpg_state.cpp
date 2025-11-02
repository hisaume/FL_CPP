#include <FL_General.h>
#include <rpg_state.h>
#include <assert.h>


RPG_ZONE* RPG_State::CurrentZone()
{
	auto z = zonelist.find( current_zone );
	if( z == zonelist.end() ) {
		ERR( current_zone, L"CRITICAL - STATE::CurrentZone: current zone is not found in the zonelist.\n" );
		// This is an error, because a game world can't exist without a current zone.
		bool CurrentZone_does_not_exist = true;
		assert( false == CurrentZone_does_not_exist );
		return nullptr;	// error.
	}
	return &z->second;
}

RPG_ZONE* RPG_State::GetZone( const string& zone_id )
{
	auto z = zonelist.find( current_zone );
	if( z == zonelist.end() )
		return nullptr;	// NOT an error.
	return &z->second;
}

LIFE_OBJECT* RPG_State::GetLife( const string& guid, bool& _is_party )
{
	/*	Iterate through all lists.
		Order:	Party, Current zone, the rest of the zones.		*/
	_is_party = false;

	/*-----------
		Party
	-----------*/
	for( auto iter=partylist.begin(); iter!=partylist.end(); ++iter )
		if( iter->first == guid ) {
			_is_party = true;
			return &iter->second;
		}

	/*------------------
		Current Zone
	------------------*/
	RPG_ZONE *zone = CurrentZone();
	for( auto iter=zone->lifelist.begin(); iter!=zone->lifelist.end(); ++iter )
		if( iter->first == guid )
			return &iter->second;
	/*------------------
		Other Zones
	------------------*/
	for( auto ziter=zonelist.begin(); ziter!=zonelist.end(); ++ziter ) {
		if( ziter->first == current_zone )
			continue;
		for( auto iter=ziter->second.lifelist.begin(); iter!=ziter->second.lifelist.end(); ++iter )
			if( iter->first == guid )
				return &iter->second;
	}
	return nullptr;
}

bool RPG_State::DeleteLife( const string& guid )
{
	/*	Iterate through all lists then delete if guid is found.
		Order:	Party, Current zone, the rest of the zones.		*/
	/*-----------
		Party
	-----------*/
	for( auto iter=partylist.begin(); iter!=partylist.end(); ++iter )
		if( iter->first == guid ) {
			partylist.erase( iter );
			return true;
		}
	/*------------------
		Current Zone
	------------------*/
	RPG_ZONE *zone = CurrentZone();
	for( auto iter=zone->lifelist.begin(); iter!=zone->lifelist.end(); ++iter )
		if( iter->first == guid ) {
			zone->lifelist.erase( iter );
			return true;
		}
	/*------------------
		Other Zones
	------------------*/
	for( auto ziter=zonelist.begin(); ziter!=zonelist.end(); ++ziter ) {
		if( ziter->first == current_zone )
			continue;
		for( auto iter=ziter->second.lifelist.begin(); iter!=ziter->second.lifelist.end(); ++iter )
			if( iter->first == guid ) {
				ziter->second.lifelist.erase( iter );
				return true;
			}
	}
	return false;
}
