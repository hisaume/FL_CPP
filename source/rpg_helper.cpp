#include <rpg_helper.h>
#include <rpg_database.h>

/*-------------------------
	Namespace HELPER
-------------------------*/
namespace HELPER
{

void Inventory_CreateEquipSlotRestrictedList( RPG_INVENTORY &inv, std::deque<RPG_EQUIPSLOTS::EQUIPSLOT> &slot
											, std::deque<int> *conversion_list, std::deque<string> *item_id_list )
{
	if( slot.empty() )	// ERROR evasion.
		return;	// reject if not slot restricted.

	RPG_EQUIPSLOTS::EQUIPSLOT *equipslot;
	int idx=0;
	for( auto iter=inv.content.begin(); iter!=inv.content.end(); ++iter, ++idx ) {
		if( !iter->Empty() ) {
			for( auto slotiter=slot.begin(); slotiter!=slot.end(); ++slotiter ) {
				equipslot = &DATABASE->Item[iter->guid].equipment.slot;
				if( *slotiter == *equipslot ) { // if it's the same slot...
					conversion_list->push_back( idx );	// record index
					if( item_id_list )
						item_id_list->push_back( iter->guid ); // record item id
					break;
				}
			}
		}
	}
}

void Inventory_CreateEquipSlotAndWearRestrictedList( RPG_INVENTORY &inventory
													, std::deque<RPG_EQUIPSLOTS::EQUIPSLOT> &slot
													, std::map<string,int> &wear
													, std::deque<int> *conversion_list, std::deque<string> *item_id_list )
{
	if( slot.empty() || wear.empty() )	// ERROR evasion.
		return;		// reject if not slot or wear restricted.

	Inventory_CreateEquipSlotRestrictedList( inventory, slot, conversion_list, item_id_list );
	/*
		Now conversion_list and item_id_list are restricted by slots.
		Restrict them further by 'wear' category.
	*/
	if( item_id_list->empty() )
		return;
	std::deque<int> tConvList;		// temp list
	std::deque<string> tItemList;	// temp list
	auto iConv = conversion_list->begin();
	for( auto iter=item_id_list->begin(); iter!=item_id_list->end(); ++iter ) {
		for( auto itw=wear.begin(); itw!=wear.end(); ++itw ) {
			if( itw->first == DATABASE->Item[*iter].equipment.wear_category ) {
				// Check character's wear level has reached (or exceeded) the requirement.
				if( itw->second >= DATABASE->Item[*iter].equipment.wear_category_level ) {
					tItemList.push_back( *iter );
					tConvList.push_back( *iConv );
				}
				break;
			}
		}
		++iConv;
	}
	*conversion_list = tConvList;
	*item_id_list = tItemList;
}

} // END of namespace HELPER
