#ifndef __RPG_HELPER_H__
#define __RPG_HELPER_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_helper.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

	Helper functions.
	
	General rule is data is not altered in these functions;
	marely rearranged or interpreted.

	Data manipulation functions should reside in PROCESS namespace.
	See rpg_processor_data.h file.

-----------------------------------------------------------------*/
#include <rpg_data_basic.h>
#include <rpg_state.h>

namespace HELPER
{

	/* Pass a list of equip slots and it will return with 'conversion_list' and 'item_id_list',
		a newly created list which is restricted by specific equip slot(s).
		'conversion_list' contains the real slot index of 'inventory.content'
		so the real location can be accessed by content[conversion_list[index]],
		where index is the array index of conversion_list and item_id_list (which correlate to each other). */
	void Inventory_CreateEquipSlotRestrictedList( RPG_INVENTORY &inventory, std::deque<RPG_EQUIPSLOTS::EQUIPSLOT> &slot,
													std::deque<int> *conversion_list, std::deque<string> *item_id_list =nullptr );
	/* As Inventory_CreateEquipSlotRestrictedList(), it narrows down inventory listing by slot,
		but this method narrows it down further by wear categories. All parameters must be
		provided for this method to function correctly.
		Return values are 'conversion_list' and 'item_id_list'.
	*/
	void Inventory_CreateEquipSlotAndWearRestrictedList( RPG_INVENTORY &inventory
														, std::deque<RPG_EQUIPSLOTS::EQUIPSLOT> &slot
														, std::map<string,int> &wear
														, std::deque<int> *conversion_list, std::deque<string> *item_id_list );
}
/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__RPG_HELPER_H__