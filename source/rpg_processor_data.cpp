#include "rpg_processor_data.h"
#include <FL_General.h>
#include <rpg_database.h>

/*-------------------------
	Namespace PROCESS
-------------------------*/
namespace PROCESS
{


/*-------------------------

		INVENTORY

-------------------------*/
bool Inventory_ItemIn( RPG_INVENTORY &inventory, const string& item_id, int& quantity, int stack_size_override )
{
	auto &content = inventory.content;
	auto &max_size = inventory.max_size;

	RPG_INVENTORYSLOT *slot = nullptr;
	int stack_size;
	if( stack_size_override <= 0 )
		stack_size = DATABASE->Item[item_id].stack_size;
	else
		stack_size = stack_size_override;

	// Find 1st stack with room to add.
	for( auto iter=content.begin(); iter!=content.end(); ++iter )
		if( iter->guid == item_id && iter->quantity < stack_size ) {
			slot = &(*iter);
			break;
		}

	if( nullptr == slot ) {
		// item_id doesn't exist in this bag. Need to create a new stack, but do we have room?
		RPG_INVENTORYSLOT *new_stack;
		if( nullptr == (new_stack=inventory.GetFirstEmptySlot()) )
			return false;	// inventory completely full.
		// So we have room to create a stack.
		new_stack->guid = item_id;
		if( quantity >= stack_size ) {
			new_stack->quantity = stack_size;
			quantity -= stack_size;	// adjust quantity. This is the remainder.
		}
		else {
			new_stack->quantity = quantity;
			quantity = 0; // complete insertion.
		}
	}
	else {	// Existing slot found. Add quantity to it.
		slot->quantity += quantity;
		if( slot->quantity > stack_size ) {
			quantity = slot->quantity - stack_size; // adjust quantity.
			slot->quantity = stack_size; // full stack.
		}
		else
			quantity = 0; // complete insertion.
	}
	return true;
}

bool Inventory_ItemIn_Q1( RPG_INVENTORY &inventory, const string& item_id, int stack_size_override )
{
	int i=1;
	return Inventory_ItemIn( inventory, item_id, i, stack_size_override );
}

bool Inventory_ItemIn_Q1( RPG_INVENTORY &inventory, const string& item_id )
{
	int i=1;
	return Inventory_ItemIn( inventory, item_id, i, 0 );
}

bool Inventory_ItemOut( RPG_INVENTORY &inventory, const string& item_id )
{
	auto &content = inventory.content;

	if( content.empty() )
		return false;

	// Find the LAST entry of this item (ie. from the back of the inventory).
	bool found = false;
	auto iter = content.end();
	do {
		--iter;
		if( iter->guid == item_id ) {
			found = true;
			break;
		}
	} while( iter!=content.begin() );

	if( !found )
		return false; // item_id not found.

	// Take 1 off the quantity and remove stack if needed.
	--iter->quantity;
	if( iter->quantity <= 0 ) // if stack is reduced to 0, remove the whole stack.
		iter->Clear();
	return true;
}

void Inventory_Swap( RPG_INVENTORY &inventory, int index1, int index2 )
{
	auto &content = inventory.content;

	if( index1 == index2 )
		return;
	/*	Visualization aid:

			slot1
			slot2		<--- index2 (or 1)
			slot3
			...
			slot9		<--- index1 (or 2)
			slot10							*/
	auto &iter1 = content.begin();
	auto &iter2 = content.begin();
	inventory.GetSlotIterator( index1, iter1 );
	inventory.GetSlotIterator( index2, iter2 );

	if( iter1->Empty() && iter2->Empty() )	// both slots empty so no point.
		return;
	if( iter1->guid != iter2->guid ) {
		/*	Different item_id	*/
		iter2->SwapContent( *iter1 ); // swap content between 1 and 2.
	}
	else {
		/*	Same item_id	*/
		int stack_size = DATABASE->Item[iter2->guid].stack_size;
		// Fill index2 first, then remainder stay in index1.
		iter2->quantity += iter1->quantity;
		if( iter2->quantity > stack_size ) { // if over capacity...
			iter1->quantity = iter2->quantity - stack_size;	// remainder in index1
			iter2->quantity = stack_size;					// 1 complete stack for index2
		}
		else { // so index2 managed to take everything i.e. index1 moved entirely to index2.
			*iter1 = RPG_INVENTORYSLOT(); // empty out index1.
		}
	}
}

int Inventory_ReduceQuantity( RPG_INVENTORY &inventory, std::list<RPG_INVENTORYSLOT>::iterator slot, int quantity )
{
	int remainder;
	if( slot->quantity > quantity ) { // if inventory has more than the required quantity...
		remainder = 0;
		slot->quantity -= quantity;	// adjust stock
	}
	else { // if inventory has less than or equal to the required quantity...
		// This results in the elimination of this slot as the stock is depleted completely.
		remainder = quantity - slot->quantity;
		inventory.content.erase( slot );	// eliminate slot.
		inventory.content.push_back( RPG_INVENTORYSLOT() );	// replace it with new empty slot.
	}
	return remainder;
}

int Inventory_ReduceQuantity( RPG_INVENTORY &inventory, int slot_index, int reduction_quantity )
{
	std::list<RPG_INVENTORYSLOT>::iterator slotIter;
	inventory.GetSlotIterator( slot_index, slotIter );
	return Inventory_ReduceQuantity( inventory, slotIter, reduction_quantity );
}

/*-------------------------

		A.I.

-------------------------*/
bool LIFE_Equip( LIFE_OBJECT *life_obj, const string& item_id, bool _party_inventory )
{
	// Set designated inventory. If not party, individual inventory is used.
	RPG_INVENTORY *inventory;
	if( _party_inventory )
		inventory = &STATE->inventory_equip;
	else {
		inventory = &life_obj->pnp.inventory;
	}

	// Find the LAST slot where item_id reside.
	int slot_index = inventory->max_size-1;
	auto iter = inventory->content.rbegin();
	for( ; iter!=inventory->content.rend(); ++iter ) {
		if( iter->guid == item_id )
			break;
		--slot_index;
	}
	if( iter == inventory->content.rend() ) {
		ERR( item_id, L"New item to be equiped does not exist in the specified inventory. Equip failed.\n" );
		return false;
	}
	return LIFE_EquipByInventorySlot( life_obj, slot_index, _party_inventory );
}

bool LIFE_EquipByInventorySlot( LIFE_OBJECT *life_obj, int slot_index, bool _party_inventory
								, RPG_EQUIPSLOTS::EQUIPSLOT target_slot )
{
	/*	NOTE1:	Take item_id out of the designated inventory, usually from party inventory,
				but for monsters _party_inventory=false and individual's inventory is used. 
		NOTE2:	Single slot equipment would be a direct swap.
		NOTE3:	Complications occur when incoming or outgoing item is a 2 hander weapon.		*/

	// Set designated inventory. If not party, individual inventory is used.
	RPG_INVENTORY *inventory;
	if( _party_inventory )
		inventory = &STATE->inventory_equip;
	else {
		inventory = &life_obj->pnp.inventory;
	}

	// Find what item is in the specified slot.
	string item_id = inventory->GetSlot(slot_index)->guid;

	// Decide which slot the equipment should go into. This may be user specified in 'target_slot'.
	RPG_EQUIPSLOTS::EQUIPSLOT	which_slot;
	if( target_slot < RPG_EQUIPSLOTS::SLOT_SIZE ) {
		which_slot = target_slot;
		/*
			There is a quirk here: If the incoming item is a 2 hander, 
			the process needs to know it's a 2 hander i.e. TWOHAND
			otherwise it will be treated as a single slot equipment.
			Insertion into right hand is assumed as that's the only place a 2 hander can go.
		*/
		if( DATABASE->Item[item_id].equipment.slot == RPG_EQUIPSLOTS::TWOHAND )
			which_slot = RPG_EQUIPSLOTS::TWOHAND;
	}
	else {
		// Ensure the incoming item defines which equipment slot it goes into. If not, can't attempt to equip it.
		which_slot = DATABASE->Item[item_id].equipment.slot;
		if( which_slot == RPG_EQUIPSLOTS::SLOT_SIZE ) {	// this error would have come from database XML file OR if not an equipment.
			ERR( item_id, L"New item to be equiped does not define which slot it goes into. Equip failed.\n" );	// slot undefined.
			return false;
		}
	}

	// Take item_id out of inventory (if this item exists!).
	if( 0 != PROCESS::Inventory_ReduceQuantity(*inventory,slot_index,1) ) {
		ERR( item_id, L"New item to be equiped does not exist in the given inventory. Equip failed. @slot_index " );
		LOGVAR( slot_index, true );
		return false;
	}

	/*------------------------------
			Weapon processing
			(complicated part)
	------------------------------*/
	if( which_slot == RPG_EQUIPSLOTS::EITHERHAND )
	{
		/* What's the current state of equipment in both hands? */
		bool right_occupied=true, left_occupied=true;
		if( life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND].empty() )
			right_occupied = false;
		if( life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND].empty() )
			left_occupied = false;

		// If 2-hander...
		if( DATABASE->Item[life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND]].equipment.slot
			== RPG_EQUIPSLOTS::TWOHAND ) {
			// Drop this 2 hander into inventory. 2 hander always in RHAND.
			if( false == PROCESS::Inventory_ItemIn_Q1( *inventory, life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] ) ) {
				ERR( life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND], L"Unequiped 2-hander lost in AI_Equip due to Inventory_ItemIn returning false unexpectedly.\n" );
				/*
					TODO:	Drop this item onto ground, but through messaging system.
				*/
			}
			life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] = item_id; // 1 hander into right hand.
			return true;
		}
		/*
			So we can assume there's no 2 hander.
		*/
		if( !right_occupied ) // if right hand is free...
			life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] = item_id; // 1 hander into right hand.
		else if( !left_occupied )	// if left hand is free...
			life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND] = item_id; // 1 hander into left hand.
		else {	// both hands occupied.
			/*
				Auto-determine where best the weapon should go, right or left hand.
				Take lower atk weapon off and place new weapon in.
			*/
			int right_atk = DATABASE->Item[life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND]].equipment.atk_physical
							+ DATABASE->Item[life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND]].equipment.atk_elemental;
			int left_atk = DATABASE->Item[life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND]].equipment.atk_physical
							+ DATABASE->Item[life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND]].equipment.atk_elemental;
			string item_takenoff;
			if( left_atk < right_atk ) { // If left hand is weaker...
				item_takenoff = life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND];
				life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND] = item_id; // put new item in left hand.
			}
			else { // If right hand is weaker OR both hands are the same...
				item_takenoff = life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND];
				life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] = item_id; // put new item in right hand.
			}
			if( false == item_takenoff.empty() ) {
				if( false == PROCESS::Inventory_ItemIn_Q1(*inventory,item_takenoff) ) {
					ERR( life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND], L"Unequiped 2-hander lost in AI_Equip due to Inventory_ItemIn returning false unexpectedly.\n" );
					/*
						TODO:	Drop this item onto ground, but through messaging system.
					*/
				}
			}
		}
		return true;
	}
	else if( which_slot == RPG_EQUIPSLOTS::TWOHAND ) { // If incoming item is a 2 hander...
		/*
			Note how this process assumes that a 2 hander always goes into right hand slot.
		*/
		// Check if both hands need unequipping in which case we need 2 slots in the inventory to unequip.
		if( !life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND].empty()
			&& !life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND].empty() ) {
			if( inventory->AvailableSize() < 2 ) {	// do we have room for 2?
				PROCESS::Inventory_ItemIn_Q1(*inventory,item_id);	// No error check ok.
				return false; // no room to take off existing items.
			}
		}
		// Empty both hands. We can assume inventory has room for both ops.
		if( false == life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND].empty() )
			PROCESS::Inventory_ItemIn_Q1( *inventory, life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] ); // No error check ok.
		if( false == life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND].empty() ) {
			PROCESS::Inventory_ItemIn_Q1( *inventory, life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND] ); // No error check ok.
			life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND].clear();
		}
		life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] = item_id;	// equip new 2 hander.
		return true;
	}
	else if( which_slot == RPG_EQUIPSLOTS::RHAND ) { // If incoming item is a right hander...
		if( false == life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND].empty() )
			PROCESS::Inventory_ItemIn_Q1( *inventory, life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] ); // No error check ok.
		life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] = item_id;
		return true;
	}
	else if( which_slot == RPG_EQUIPSLOTS::LHAND ) { // If incoming item is an off hander...
		// If right hand has a 2-hander...
		if( DATABASE->Item[life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND]].equipment.slot == RPG_EQUIPSLOTS::TWOHAND ) {
			/*	Remove this 2 hander into inventory then equip the left hander.
				No need to check left hand because 2 hander means nothing is in left hand.
			*/
			PROCESS::Inventory_ItemIn_Q1( *inventory, life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND] ); // dispose 2 hander into inventory.
			life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::RHAND].clear(); // empty out 2 hander in right hand.
			life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND] = item_id; // equip the left hand.
			return true;
		}
		// So it's not a 2 hander.
		if( false == life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND].empty() )
			PROCESS::Inventory_ItemIn_Q1( *inventory, life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND] ); // No error check ok.
		life_obj->pnp.equipment.slot[RPG_EQUIPSLOTS::LHAND] = item_id;
		return true;
	}
	/*-------------------------------------------
				Armour processing
		Non weapon, single slot to single slot.
	-------------------------------------------*/
	if( false == life_obj->pnp.equipment.slot[which_slot].empty() )
		PROCESS::Inventory_ItemIn_Q1( *inventory, life_obj->pnp.equipment.slot[which_slot] ); // No error check ok.
	life_obj->pnp.equipment.slot[which_slot] = item_id;
	return true;
}

int LIFE_Unequip( LIFE_OBJECT *life, RPG_EQUIPSLOTS::EQUIPSLOT slot, bool _is_party )
{
	string &item_id = life->pnp.equipment.slot[slot]; // what item_id is in the slot?
	if( item_id.empty() )
		return -1;	// bail out as an item doesn't exist in this slot.

	RPG_INVENTORY *inv;
	if( _is_party )						// If party member...
		inv = &STATE->inventory_equip;	//		use party equipment inventory
	else								// If non party member...
		inv = &life->pnp.inventory;		//		use his own inventory space

	// Put the item_id into the inventory to see if it goes in.
	if( false == PROCESS::Inventory_ItemIn_Q1(*inv,item_id) )
		return -2;	// bag is full!! Issue text to the player and bail out.

	// Item has been put in the bag, so now unequip it off the character.
	item_id.clear();

	return 1;	// return above 0 for success.
}


} // END of Namespace PROCESS
