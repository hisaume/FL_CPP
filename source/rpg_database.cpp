
#include "rpg_database.h"
#include "rpg_xml_dataloader.h"


void RPG_Database::TEST_LoadHardcoded()
{
	
	/*-----------
		ITEMS
	-----------*/

	/*
	{
		RPG_ITEM item;
		item.guid = "item_yakusou";
		item.default_name = L"やくそう";
		item.default_desc = L"ＨＰ回復・小";
		item.item_type	= RPG_ITEM::ITEM;
		item.quest_item	= false;
		item.stack_size	= 20;
		item.pricetag	= 10;
		item.use_effect.actual_effect = "insta_heal_hp";
		item.use_effect.value		= 10;
		item.use_effect.offensive_effect = false;
		Item[item.guid] = item;
	}
	{
		RPG_ITEM item;
		item.guid = "item_dokukeshisou";
		item.default_name = L"どくけしそう";
		item.default_desc = L"どくちりょう・小";
		item.item_type	= RPG_ITEM::ITEM;
		item.quest_item	= false;
		item.stack_size	= 20;
		item.pricetag	= 10;
		item.use_effect.actual_effect = "insta_heal_poison";
		item.use_effect.value		= 10;
		item.use_effect.offensive_effect = false;
		Item[item.guid] = item;
	}
	{
		RPG_ITEM item;
		item.guid = "item_mahounoyakusou";
		item.default_name = L"まほうやくそう";
		item.default_desc = L"ＭＰ回復・小";
		item.item_type	= RPG_ITEM::ITEM;
		item.quest_item	= false;
		item.stack_size	= 20;
		item.pricetag	= 10;
		item.use_effect.actual_effect = "insta_heal_mp";
		item.use_effect.value		= 10;
		item.use_effect.offensive_effect = false;
		Item[item.guid] = item;
	}
	{
		RPG_ITEM item;
		item.guid = "item_kayakudan";
		item.default_name = L"かやく弾";
		item.default_desc = L"無属ダメージ・小";
		item.item_type	= RPG_ITEM::ITEM;
		item.quest_item	= false;
		item.stack_size	= 20;
		item.pricetag	= 10;
		item.use_effect.actual_effect = "insta_damage_physical";
		item.use_effect.value		= 10;
		item.use_effect.offensive_effect = true;
		Item[item.guid] = item;
	}*/

	/*------------
		SPELLS
	------------*/
	/*
	{
		RPG_SPELL spell;
		spell.guid = "spell_hoimi";
		spell.default_name = L"ホイミ";
		spell.default_desc = L"ＨＰ回復・小";
		spell.spell_effect.effect_width = spell.spell_effect.SINGLE;
		spell.spell_effect.actual_effect = "insta_heal_hp";
		spell.spell_effect.value = 20;
		spell.cast_timer.SetDuration( 0 );
		spell.refresh_timer.SetDuration( 5 );
		Spell[spell.guid] = spell;
	}
	{
		RPG_SPELL spell;
		spell.guid = "spell_mera";
		spell.default_name = L"メラ";
		spell.default_desc = L"火属ダメージ・小";
		spell.spell_effect.effect_width = spell.spell_effect.SINGLE;
		spell.spell_effect.actual_effect = "insta_damage_fire";
		spell.spell_effect.value = 20;
		spell.cast_timer.SetDuration( 0 );
		spell.refresh_timer.SetDuration( 10 );
		Spell[spell.guid] = spell;
	}*/
}

void RPG_Database::LoadXML( string xmlfile )
{
	DataLoaderXML loader;
	loader.Parse( xmlfile, &Item, &Spell, &Class, &Bagtype );
	/*
		Update string table stored within this database.
		Don't clear the string table. Just overwrite
		because LoadXML may be called multiple times
		for each of Item, Spell, etc.

		Note how default name AND description are named for string table.

		e.g. For an item with an id "item_yakusou":

				StringTable["item_yakusou"] is the name and
				StringTable["item_yakusou_desc"] is the description.
	*/
	for( auto iter=Item.begin(); iter!=Item.end(); ++iter ) {
		StringTable[iter->first] = iter->second.default_name;
		StringTable[iter->first+"_desc"] = iter->second.default_desc;
	}
	for( auto iter=Spell.begin(); iter!=Spell.end(); ++iter ) {
		StringTable[iter->first] = iter->second.default_name;
		StringTable[iter->first+"_desc"] = iter->second.default_desc;
	}
	for( auto iter=Class.begin(); iter!=Class.end(); ++iter ) {
		StringTable[iter->first] = iter->second.default_name;
		StringTable[iter->first+"_desc"] = iter->second.default_desc;
	}
	for( auto iter=Bagtype.begin(); iter!=Bagtype.end(); ++iter ) {
		StringTable[iter->first] = iter->second.default_name;
		StringTable[iter->first+"_desc"] = iter->second.default_desc;
	}
}

bool RPG_Database::IsEquipment( string item_id )
{
	switch( Item[item_id].item_type )
	{
	case RPG_ITEM::TYPE_SIZE:	// <--- this shouldn't be necessary.
	case RPG_ITEM::UNKNOWN:
	case RPG_ITEM::ITEM:
		return false;	// NOT equipment!
	default:
		return true;
	}
}
