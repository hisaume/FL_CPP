#ifndef __RPG_PROCESSOR_DATA_H__
#define __RPG_PROCESSOR_DATA_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_processor_data.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

	Processer functions.
	
	Typically they alter data at fundamental level and are
	usually only called during command processing where
	state change takes place.

	For a sister namespace HELPER, which only rearrange or
	interpret data instead of change them, see rpg_helper.h file.

-----------------------------------------------------------------*/
#include <rpg_data_basic.h>
#include <rpg_state.h>


namespace PROCESS
{
	/*-----------------------
			INVENTORY
	-----------------------*/
	/* Attempts to add 'quantity' in the 1st available stack of the same item_id.
		'quantity' will contain the amount which could not be inserted in that stack,
		meaning a complete insertion will return 0. Set 'stack_size_override' to
		a desired stack size when overriding the default given by the DATABASE.
		@Remark Call this method repeatedly with remaining quantity for complete
		insertion, though if false is returned the inventory is full up.
		@Returns false if there is no more slots left in the inventory.		*/
	bool Inventory_ItemIn( RPG_INVENTORY &inventory, const string& item_id, int& quantity, int stack_size_override =0 );
	/* Specialized case of Inventory_ItemIn, where 'quantity' is only 1.
		@Return false if the given item did not go into the inventory.	*/
	bool Inventory_ItemIn_Q1( RPG_INVENTORY &inventory, const string& item_id, int stack_size_override );
	bool Inventory_ItemIn_Q1( RPG_INVENTORY &inventory, const string& item_id );
	/* Takes 1 of item_id off the inventory. Stack management is done automatically.
		@Returns false if item_id was not found.	*/
	bool Inventory_ItemOut( RPG_INVENTORY &inventory, const string& item_id );
	/* Process where index2 is cast over index 1. Think FF where 1st choice item (index1) is put over
		2nd choice item (index2).
		@Remark If the contained items are the same for both indices, a merger takes place. Index2 will be
		filled up as much as possible and the left over will remain in index1.	*/
	void Inventory_Swap( RPG_INVENTORY &inventory, int index1, int index2 );
	/* Automatically manages the consequence of stock reduction from a particular slot (given by 'slot_iter')
		of an inventory (given by 'inventory'), including the removal of the slot.
		@RETURN the quantity which was NOT reduced if the slot does not contain enough stock i.e. full reduction would
			always return 0.
		@REMARK 'slot_iter' must be one of the slots in 'inventory'. They can't be different. */
	int Inventory_ReduceQuantity( RPG_INVENTORY &inventory, std::list<RPG_INVENTORYSLOT>::iterator slot_iter, int reduction_quantity );
	int Inventory_ReduceQuantity( RPG_INVENTORY &inventory, int slot_index, int reduction_quantity );

	/*-----------------------
			LIFE
	-----------------------*/
	/* Variation of LIFE_EquipByInventorySlot().
		Equips item_id on life_obj and unequiped item is put in the inventory.
		Item to be equiped must first be in the inventory.
		'_party_inventory' determines which inventory is to be used;
		set true for unified party equipment inventory or
		false to use the life object's personal inventory (usually for NPCs).
	*/
	bool LIFE_Equip( LIFE_OBJECT *life_obj, const string& item_id, bool _party_inventory =true );
	/* Equips an item FROM a particular slot of an inventory. Which equipment slot
		(of the given character) it should go into is worked out internally.
		'_party_inventory' determines which inventory is to be used;
		set true for unified party equipment inventory or
		false to use the life object's personal inventory (usually for NPCs).
		If 'target_slot' is set to SLOT_SIZE, the incoming equipment will be placed in
		the appropriate slot automatically. Otherwise, specify where you want it to go into.
		NOTE: Skill/Ability checks etc is NOT done!!
	*/
	bool LIFE_EquipByInventorySlot( LIFE_OBJECT *life_obj, int source_slot_index, bool _party_inventory
									, RPG_EQUIPSLOTS::EQUIPSLOT target_slot =RPG_EQUIPSLOTS::SLOT_SIZE );
	/* Unequips the specified slot of the given character and
		puts the unequiped item into an appropriate inventory.
		@return ABOVE 0 for success, -1 if specified slot is empty already, -2 if inventory is full.
	*/
	int LIFE_Unequip( LIFE_OBJECT *life_obj, RPG_EQUIPSLOTS::EQUIPSLOT slot, bool is_party );
}

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif //__RPG_PROCESSOR_DATA_H__