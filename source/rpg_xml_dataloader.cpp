#include "rpg_xml_dataloader.h"

#include <tinyxml.h>
#include <FL_General.h>
#include <FL_GeneralTools.h>	// for LONG2STRING and CHAR2LONG function
#include <FL_File.h>

using namespace std;


void DataLoaderXML::Parse( std::string filename, std::map<string,RPG_ITEM> *_item
												, std::map<string,RPG_SPELL> *_spell
												, std::map<string,RPG_CLASS_DEFINITION>	*_class
												, std::map<string,RPG_BAGTYPE> *_bagtype )
{
	m_Item = _item;
	m_Spell = _spell;
	m_Class = _class;
	m_Bagtype = _bagtype;

	TiXmlDocument   *XMLDoc = 0;

	try
	{
		if( !FL::FL_FileExistsC( filename ) ) {
			// file does not exist. Error.
			return;
		}

		std::string stream = FL::FL_FileReadStringC( filename );
		LOGVAR( filename ); LOG( L" File opened.\n" );
		XMLDoc = new TiXmlDocument();
		XMLDoc->Parse( stream.c_str(), 0, TIXML_ENCODING_UTF8 );

		if( XMLDoc->Error() )
		{
			//We'll just log, and continue on gracefully
			ERR( L"[DataLoaderXML] " );
			ERR( FL::ASCII_to_UTF8(XMLDoc->ErrorDesc()) );
			LOG( L" ROW" );
			LOGVAR( XMLDoc->ErrorRow() );
			LOG( L" COL" );
			LOGVAR( XMLDoc->ErrorCol(), true );
			delete XMLDoc;
			return;
		}
	}
	catch(...)
	{
		//We'll just log, and continue on gracefully
		ERR( L"[DataLoaderXML] Error creating TiXmlDocument" );
		delete XMLDoc;
		return;
	}

	TiXmlElement   *XMLRoot;
	XMLRoot = XMLDoc->RootElement();

	// Validate the File
	if( std::string(XMLRoot->Value()) != "flxml"  ) {
		ERR( L"[DataLoaderXML] Error: Invalid xml file. Missing <flxml>.\n" );
		delete XMLDoc;
		return;
	}

	LOGVAR( filename ); LOG( L"Parsing in progress.\n" );

	string version = getAttrib(XMLRoot, "formatVersion", "unknown");

	TiXmlElement *pElement;

	/*-----------------
		<itemtable>
	-----------------*/
	pElement = XMLRoot->FirstChildElement( "itemtable" );
	while( pElement )
	{
		ProcessItemTable( pElement );
		pElement = pElement->NextSiblingElement( "itemtable" );
	}
	/*-----------------
		<spelltable>
	-----------------*/
	pElement = XMLRoot->FirstChildElement( "spelltable" );
	while( pElement )
	{
		ProcessSpellTable( pElement );
		pElement = pElement->NextSiblingElement( "spelltable" );
	}
	/*-----------------
		<classtable>
	-----------------*/
	pElement = XMLRoot->FirstChildElement( "classtable" );
	while( pElement )
	{
		ProcessClassTable( pElement );
		pElement = pElement->NextSiblingElement( "classtable" );
	}

	/*-----------------
		<bagtypetable>
	-----------------*/
	pElement = XMLRoot->FirstChildElement( "bagtypetable" );
	while( pElement )
	{
		ProcessBagtypeTable( pElement );
		pElement = pElement->NextSiblingElement( "bagtypetable" );
	}

	// Close the XML File
	delete XMLDoc;
}

void DataLoaderXML::ProcessItemTable( TiXmlElement *XMLRoot )
{
	TiXmlElement *pElement;

	pElement = XMLRoot->FirstChildElement( "item" );
	while( pElement )
	{
		ProcessItemSegment( pElement );
		pElement = pElement->NextSiblingElement( "item" );
	}
}

void DataLoaderXML::ProcessSpellTable( TiXmlElement *XMLRoot )
{
	TiXmlElement *pElement;

	pElement = XMLRoot->FirstChildElement( "spell" );
	while( pElement )
	{
		ProcessSpellSegment( pElement );
		pElement = pElement->NextSiblingElement( "spell" );
	}
}

void DataLoaderXML::ProcessClassTable( TiXmlElement *XMLRoot )
{
	TiXmlElement *pElement;

	pElement = XMLRoot->FirstChildElement( "class" );
	while( pElement )
	{
		ProcessClassSegment( pElement );
		pElement = pElement->NextSiblingElement( "class" );
	}
}

void DataLoaderXML::ProcessBagtypeTable( TiXmlElement *XMLRoot )
{
	TiXmlElement *pElement;

	pElement = XMLRoot->FirstChildElement( "bagtype" );
	while( pElement )
	{
		ProcessBagtypeSegment( pElement );
		pElement = pElement->NextSiblingElement( "bagtype" );
	}
}

void DataLoaderXML::ProcessItemSegment( TiXmlElement *XMLNode )
{
	RPG_ITEM item;

	/*	Attributes:
			id, type, usability, stack_size, quest_item, pricetag
		Child Segments:
			<default>, <effect>, <equipment>
	*/
	
	/*-------------------------
		Extract attributes.
	-------------------------*/
	item.guid = getAttrib( XMLNode, "id", "_id_nonexistent" );
	if( item.guid.empty() ) {
		WARN( L"DataLoaderXML <item>: An item has an empty id. It must be set to a unique value. Skipping.\n" );
		return;
	}
	else if( item.guid == "_id_nonexistent" ) {
		WARN( L"DataLoaderXML <item>: An item has not defined an id. It must have an id. Skipping.\n" );
		return;
	}

	string type = getAttrib( XMLNode, "type" );
	if( type.empty() )
		item.item_type = RPG_ITEM::ITEM;	// default
	else if( type == "item" )
		item.item_type = RPG_ITEM::ITEM;
	else if( type == "weapon" )
		item.item_type = RPG_ITEM::WEAPON;
	else if( type == "armour" )
		item.item_type = RPG_ITEM::ARMOUR;
	else if( type == "accessory" )
		item.item_type = RPG_ITEM::ACCESSORY;
//	else if( type == "shield" )
	//	item.item_type = RPG_ITEM::SHIELD;
//	else if( type == "body" )
//		item.item_type = RPG_ITEM::BODY;
	else {
		WARN( FL::ASCII_to_UTF8(type), L"DataLoaderXML: Unknown item type.\n" );
		item.item_type = RPG_ITEM::UNKNOWN;
	}

	string usability = getAttrib( XMLNode, "usability" );
	if( usability.empty() )
		item.usability = RPG_ITEM::NOUSE;	// default
	else if( usability == "nouse" )
		item.usability = RPG_ITEM::NOUSE;
	else if( usability == "once" )
		item.usability = RPG_ITEM::ONCE;
	else if( usability == "unlimited" )
		item.usability = RPG_ITEM::UNLIMITED;

	item.stack_size = getAttribInt( XMLNode, "stack_size", 1 );
	if( item.stack_size == -1000000 ) {
		WARN( FL::ASCII_to_UTF8(item.guid), L"DataLoaderXML: 'stack_size' must be an integer. Defaulting to 1.\n" );
		item.stack_size = 1;
	}

	item.quest_item = getAttribBool( XMLNode, "quest_item", false );

	item.price = getAttribInt( XMLNode, "price", 0 );
	if( item.price == -1000000 ) {
		WARN( FL::ASCII_to_UTF8(item.guid), L"DataLoaderXML: 'price' must be an integer. Defaulting to 0.\n" );
		item.price = 0;
	}

	/*-------------------------------
		Extract child segments
	-------------------------------*/
	TiXmlElement *pElement;

	/*-----------------
		<default>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "default" );
	if( pElement )
		ProcessDefaultSegment( pElement, &item.default_name, &item.default_desc );

	/*-----------------
		<effect>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "effect" );
	if( pElement )
		ProcessEffectSegment( pElement, &item.use_effect );

	/*-----------------
		<equipment>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "equipment" );
	if( pElement )
		ProcessEquipmentSegment( pElement, &item.equipment );
	else
		item.equipment.slot = RPG_EQUIPSLOTS::SLOT_SIZE;	// so this isn't an equipment.

	// Now 'item' is prepared. put it in the stack.
	(*m_Item)[item.guid] = item;
}

void DataLoaderXML::ProcessSpellSegment( TiXmlElement *XMLNode )
{
	RPG_SPELL	spell;
	/*	Attributes:			id
							cast_time
							refresh_time
		Child segments:
				<default>
				<effect>
	*/
	spell.guid = getAttrib( XMLNode, "id", "_non_existent" );
	spell.cast_timer.SetDuration( getAttribReal(XMLNode,"cast_time") );
	spell.refresh_timer.SetDuration( getAttribReal(XMLNode,"refresh_time") );

	TiXmlElement *pElement;
	/*-----------------
		<default>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "default" );
	if( pElement )
		ProcessDefaultSegment( pElement, &spell.default_name, &spell.default_desc );

	/*-----------------
		<effect>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "effect" );
	if( pElement )
		ProcessEffectSegment( pElement, &spell.spell_effect );

	// Now 'spell' is prepared. put it in the stack.
	(*m_Spell)[spell.guid] = spell;
}

void DataLoaderXML::ProcessClassSegment( TiXmlElement *XMLNode )
{
	RPG_CLASS_DEFINITION	classdef;
	/*
		Attributes:		id
						base_level
		Child segments:
				<default>
				<wear>
				<setstats>
				<setexp>
	*/
	classdef.guid = getAttrib( XMLNode, "id" );
	classdef.base_level = getAttribInt( XMLNode, "base_level", 1 );

	TiXmlElement *pElement;
	/*-----------------
		<default>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "default" );
	if( pElement )
		ProcessDefaultSegment( pElement, &classdef.default_name, &classdef.default_desc );

	/*-----------------
		<wear>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "wear" );
	while( pElement ) {
		string category;
		int proficiency;
		ProcessWearSegment( pElement, category, proficiency );
		classdef.wear_category[category] = proficiency;
		pElement = pElement->NextSiblingElement( "wear" );
	}

	/*-----------------
		<setstats>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "setstats" );
	while( pElement ) {
		ProcessSetstatsSegment( pElement, classdef.stats );
		pElement = pElement->NextSiblingElement( "setstats" );
	}
	/*-----------------
		<setexp>
	-----------------*/
	pElement = XMLNode->FirstChildElement( "setexp" );
	while( pElement ) {
		ProcessSetexpSegment( pElement, classdef.exp2next_lvl );
		pElement = pElement->NextSiblingElement( "setexp" );
	}

	// now classdef is ready. put it into the stack.
	(*m_Class)[classdef.guid] = classdef;
}

void DataLoaderXML::ProcessBagtypeSegment( TiXmlElement *XMLNode )
{
	/*	Attributes:		id
						size
						default_name
						default_desc	*/
	RPG_BAGTYPE bagtype;
	bagtype.bagtype_id		= getAttrib( XMLNode, "id" );
	bagtype.size			= getAttribInt( XMLNode, "size" );
	bagtype.default_name	= FL::ASCII_to_UTF8( getAttrib(XMLNode,"default_name") );
	bagtype.default_desc	= FL::ASCII_to_UTF8( getAttrib(XMLNode,"default_desc") );

	(*m_Bagtype)[bagtype.bagtype_id] = bagtype;
}

/*---------------------------------------------------

				Inner Segments

---------------------------------------------------*/
void DataLoaderXML::ProcessDefaultSegment( TiXmlElement *XMLNode, wstring *_default_name, wstring *_default_desc )
{
	/*	Attributes:		'name'
						'desc'
	*/
	*_default_name = FL::ASCII_to_UTF8( getAttrib( XMLNode,"name") );
	*_default_desc = FL::ASCII_to_UTF8( getAttrib( XMLNode,"desc") );
}

void DataLoaderXML::ProcessEffectSegment( TiXmlElement *XMLNode, RPG_EFFECT *effect )
{
	/*	Attribute:			process, element, overtime, value, duration, offensive_effect, effect_area
		Child segments:		<stats>
							<defence>
	*/
	effect->process = getAttrib( XMLNode, "process", "" );
	effect->elemental = getAttrib( XMLNode, "elemental", "neutral" );
	effect->overtime = getAttribBool( XMLNode, "overtime", false );
	effect->value = getAttribInt( XMLNode, "value", 0 );
	effect->duration.SetDuration( getAttribReal(XMLNode,"duration") );
	effect->offensive_effect = getAttribBool( XMLNode, "offensive_effect", false );

	string eff_area = getAttrib( XMLNode, "effect_area" );
	if( eff_area.empty() )
		effect->effect_area = RPG_EFFECT::SINGLE;	// default
	else if( eff_area == "single" )
		effect->effect_area = RPG_EFFECT::SINGLE;
	else if( eff_area == "all" )
		effect->effect_area = RPG_EFFECT::ALL;
	else {
		WARN( FL::ASCII_to_UTF8(eff_area), L"DataLoaderXML: Unknown 'effect_area' type in <effect>. Defaulting to single.\n" );
		effect->effect_area = RPG_EFFECT::SINGLE;
	}

	TiXmlElement *pElement;
	/*-------------------------------
		<stats> (for overtime)
	-------------------------------*/
	pElement = XMLNode->FirstChildElement( "stats" );
	while( pElement ) {
		ProcessStatsSegment( pElement, &effect->stick_stats, 0 );
		pElement = pElement->NextSiblingElement( "stats" );
	}
	/*--------------------------------
		<defence> (for overtime)
	--------------------------------*/
	pElement = XMLNode->FirstChildElement( "defence" );
	while( pElement ) {
		ProcessDefenceSegment( pElement, &effect->stick_defence );
		pElement = pElement->NextSiblingElement( "defence" );
	}
}

void DataLoaderXML::ProcessStatsSegment( TiXmlElement *XMLNode, RPG_STATS *stats, int _default )
{
	/*	Attributes:		hp, maxhp
						mp, maxmp
						strength
						stamina
						physical
						accuracy_pct
						intelligence
	*/
	stats->hp		= getAttribInt( XMLNode, "hp", _default );
	stats->maxhp	= getAttribInt( XMLNode, "maxhp", _default );
	stats->mp		= getAttribInt( XMLNode, "mp", _default );
	stats->maxmp	= getAttribInt( XMLNode, "maxmp", _default );
	stats->strength	= getAttribInt( XMLNode, "strength", _default );
	stats->stamina	= getAttribInt( XMLNode, "stamina", _default );
	//stats->physical	= getAttribInt( XMLNode, "physical", _default );
	stats->dexterity= getAttribInt( XMLNode, "dexterity", _default );
	stats->agility	= getAttribInt( XMLNode, "agility", _default );
	stats->intelligence	= getAttribInt( XMLNode, "intelligence", _default );
}

void DataLoaderXML::ProcessEquipmentSegment( TiXmlElement *XMLNode, RPG_EQUIPMENT_PROPERTY *equipment )
{
	/*	Attribute:		atk_physical
						atk_elemental
						element
						slot

		Child segment:	<wear>
						<stats>
						<defence>
	*/
	equipment->atk_physical = getAttribInt( XMLNode, "atk_physical" );
	equipment->atk_elemental = getAttribInt( XMLNode, "atk_elemental" );

	equipment->element = getAttrib( XMLNode, "element", "element_not_given" );
	if( equipment->element == "element_not_given" ) {
		// no problem.
	}
	else if( equipment->element == "neutral"
			|| equipment->element == "fire"
			|| equipment->element == "water" )
	{
		// No problem.
	}
	else {
		WARN( FL::ASCII_to_UTF8(equipment->element), L"DataLoaderXML: Unknown elemental type in <equipment>. Defaulting to no-element.\n" );
		equipment->element.clear();
	}

	string slot = getAttrib( XMLNode, "slot", "_not_given" );
	if( slot == "_not_given" || slot.empty() ) {
		WARN( L"DataLoaderXML: 'slot' is not given in <equipment> e.g. slot=\"body\"\n" );
		equipment->slot = RPG_EQUIPSLOTS::SLOT_SIZE;
	}
	else if( slot == "righthand" )
		equipment->slot = RPG_EQUIPSLOTS::RHAND;
	else if( slot == "lefthand" )
		equipment->slot = RPG_EQUIPSLOTS::LHAND;
	else if( slot == "eitherhand" )
		equipment->slot = RPG_EQUIPSLOTS::EITHERHAND;
	else if( slot == "twohand" )
		equipment->slot = RPG_EQUIPSLOTS::TWOHAND;
	else if( slot == "body" )
		equipment->slot = RPG_EQUIPSLOTS::BODY;
	else if( slot == "head" )
		equipment->slot = RPG_EQUIPSLOTS::HEAD;
	else {
		WARN( slot, L"DataLoaderXML: Unknown 'slot' value.\n" );
		equipment->slot = RPG_EQUIPSLOTS::SLOT_SIZE;
	}

	TiXmlElement		*pElement;
	/*---------------------------
		<wear> for equipment
	---------------------------*/
	pElement = XMLNode->FirstChildElement( "wear" );
	equipment->wear_category_level = 1;
	while( pElement ) {
		string category;
		int required_proficiency;
		ProcessWearSegment( pElement, category, required_proficiency );
		equipment->wear_category = category;
		equipment->wear_category_level = required_proficiency;
		pElement = pElement->NextSiblingElement( "wear" );
	}
	/*---------------------------
		<stats> for equipment
	---------------------------*/
	pElement = XMLNode->FirstChildElement( "stats" );
	while( pElement ) {
		ProcessStatsSegment( pElement, &equipment->stats, 0 );
		pElement = pElement->NextSiblingElement( "stats" );
	}
	/*------------------------------
		<defence> for equipment
	------------------------------*/
	pElement = XMLNode->FirstChildElement( "defence" );
	while( pElement ) {
		ProcessDefenceSegment( pElement, &equipment->def );
		pElement = pElement->NextSiblingElement( "defence" );
	}
}

void DataLoaderXML::ProcessDefenceSegment( TiXmlElement *XMLNode, RPG_DEFENCE *defence )
{
	/*	Attrib:		physical
					neutral
					fire
					water
	*/
	defence->physical	= getAttribInt( XMLNode, "physical" );
	defence->neutral	= getAttribInt( XMLNode, "neutral" );
	defence->fire		= getAttribInt( XMLNode, "fire" );
	defence->water		= getAttribInt( XMLNode, "water" );
}

void DataLoaderXML::ProcessWearSegment( TiXmlElement *XMLNode, string &category, int &proficiency )
{
	/*	Attrib:		category
					level

		e.g. <wear category="1hsword" level="1" />
	*/
	category	= getAttrib( XMLNode, "category" );
	proficiency	= getAttribInt( XMLNode, "level", 1 );
}

void DataLoaderXML::ProcessSetstatsSegment( TiXmlElement *XMLNode, RPG_STATS *stats_array )
{
	/*	Attributes:		method
						level
		Child segments:
				<stats>
	*/
	int level = getAttribInt( XMLNode, "level", 1 );
	string method = getAttrib( XMLNode, "method", "fill" );

	/*-------------
		<stats>
	-------------*/
	std::deque<RPG_STATS>	statslist;
	TiXmlElement			*pElement;
	pElement = XMLNode->FirstChildElement( "stats" );
	while( pElement ) {
		RPG_STATS stats;
		ProcessStatsSegment( pElement, &stats, -999999 );	// elements which weren't given will contain -999999.
		statslist.push_back( stats );
		pElement = pElement->NextSiblingElement( "stats" );
	}

	if( statslist.size() )
	{
		if( method == "bylevel" ) {
			for( int i=level, j=0; i<RPG_MAXLEVEL, j<(int)statslist.size(); ++i,++j ) {
				stats_array[i].Copy( statslist[j], -999999 );
			}
		}
		else if( method == "fill" )
		{
			for( int j=0; j<(int)statslist.size(); j++ ){
				for( int i=level; i<RPG_MAXLEVEL; ++i ) {
					if( (i+j)<RPG_MAXLEVEL )
						stats_array[i+j].Copy( statslist[j], -999999 );
				}
			}
		}
		else {
			WARN( FL::ASCII_to_UTF8(method), L"DataLoaderXML: Unknown method in <setstats>.\n" );
		}
	}
}

void DataLoaderXML::ProcessSetexpSegment( TiXmlElement *XMLNode, int *exp_array )
{
	/*	Attributes:			level
							method
		Child segments:
							<var>
	*/
	int level = getAttribInt( XMLNode, "level", 1 );
	string method = getAttrib( XMLNode, "method", "fill" );

	/*-----------
		<var>
	-----------*/
	std::deque<int>		varlist;
	TiXmlElement		*pElement;
	pElement = XMLNode->FirstChildElement( "var" );
	while( pElement ) {
		int v = 0;
		ProcessVarSegment( pElement, &v );
		varlist.push_back( v );
		pElement = pElement->NextSiblingElement( "var" );
	}

	if( varlist.size() ) {
		if( method == "bylevel" ) {
			for( int i=level, j=0; i<RPG_MAXLEVEL, j<(int)varlist.size(); ++i,++j ) {
				exp_array[i] = varlist[j];
			}
		}
		else if( method == "fill" ) {
			for( int j=0; j<(int)varlist.size(); j++ ){
				for( int i=level; i<RPG_MAXLEVEL; ++i ) {
					if( (i+j)<RPG_MAXLEVEL )
						exp_array[i+j] = varlist[j];
				}
			}
		}
		else {
			WARN( FL::ASCII_to_UTF8(method), L"DataLoaderXML: Unknown method in <setexp>.\n" );
		}
	}
}

void DataLoaderXML::ProcessVarSegment( TiXmlElement *XMLNode, int *integer_value )
{
	/*	Attributes:		int
	*/
	*integer_value = getAttribInt( XMLNode, "int", 0 );
}
/*---------------------------------------------------

			General Attribute methods

---------------------------------------------------*/
string DataLoaderXML::getAttrib(TiXmlElement *XMLNode, const string &attrib, const string &defaultValue)
{
	if( XMLNode->Attribute(attrib.c_str()) )
		return XMLNode->Attribute(attrib.c_str());
	else
		return defaultValue;
}

float DataLoaderXML::getAttribReal(TiXmlElement *XMLNode, const string &attrib, float defaultValue)
{
	double d = 0.0;
	if( XMLNode->Attribute(attrib.c_str(),&d) )
		return (float)d;
	else
		return defaultValue;
}

int DataLoaderXML::getAttribInt( TiXmlElement *XMLNode, const string &attrib, int defaultValue )
{
	int i=-1000000;
	if( XMLNode->Attribute(attrib.c_str(),&i) )
		return i;
	else
		return defaultValue;
}

bool DataLoaderXML::getAttribBool( TiXmlElement *XMLNode, const string &attrib, bool defaultValue )
{
	if(!XMLNode->Attribute(attrib.c_str()))
		return defaultValue;

	if(string(XMLNode->Attribute(attrib.c_str())) == "true")
		return true;

	return false;
}
