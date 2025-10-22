#ifndef __RPG_XML_DATALOADER_H__
#define __RPG_XML_DATALOADER_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_xml_dataloader.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

-----------------------------------------------------------------*/
#include <rpg_data_basic.h>

#include <string>
#include <map>
using namespace std;

// Forward declarations
class TiXmlElement;

class DataLoaderXML
{
public:
	DataLoaderXML()				{}
	virtual ~DataLoaderXML()	{}

	void Parse( std::string filename, std::map<string,RPG_ITEM> *_item
									, std::map<string,RPG_SPELL> *_spell
									, std::map<string,RPG_CLASS_DEFINITION>	*_class
									, std::map<string,RPG_BAGTYPE> *_bagtype );

protected:
	void ProcessItemTable( TiXmlElement *XMLRoot );
	void ProcessSpellTable( TiXmlElement *XMLRoot );
	void ProcessClassTable( TiXmlElement *XMLRoot );
	void ProcessBagtypeTable( TiXmlElement *XMLRoot );

	void ProcessItemSegment( TiXmlElement *XMLNode );
	void ProcessSpellSegment( TiXmlElement *XMLNode );
	void ProcessClassSegment( TiXmlElement *XMLNode );
	void ProcessBagtypeSegment( TiXmlElement *XMLNode );

	void ProcessDefaultSegment( TiXmlElement *XMLNode, wstring *_default_name, wstring *_default_desc );
	void ProcessEffectSegment( TiXmlElement *XMLNode, RPG_EFFECT *effect );
	void ProcessStatsSegment( TiXmlElement *XMLNode, RPG_STATS *stats, int _default_value );
	void ProcessEquipmentSegment( TiXmlElement *XMLNode, RPG_EQUIPMENT_PROPERTY *equipment );
	void ProcessDefenceSegment( TiXmlElement *XMLNode, RPG_DEFENCE *defence );
	void ProcessWearSegment( TiXmlElement *XMLNode, string &category, int &proficiency );
	void ProcessSetstatsSegment( TiXmlElement *XMLNode, RPG_STATS *stats_array );
	void ProcessSetexpSegment( TiXmlElement *XMLNode, int *exp_array );

	void ProcessVarSegment( TiXmlElement *XMLNode, int *integer_value );

	string	getAttrib( TiXmlElement *XMLNode, const string &parameter, const string &defaultValue ="" );
	/*	@Returns 0 if the attribute can't be converted to float. */
	float	getAttribReal( TiXmlElement *XMLNode, const string &parameter, float defaultValue =0 );
	/*	@Returns -1000000 (minus one million) if the attribute isn't an integer. */
	int		getAttribInt( TiXmlElement *XMLNode, const string &parameter, int defaultValue =0 );
	bool	getAttribBool( TiXmlElement *XMLNode, const string &parameter, bool defaultValue =false );

protected:
	std::map<string,RPG_ITEM>				*m_Item;
	std::map<string,RPG_SPELL>				*m_Spell;
	std::map<string,RPG_CLASS_DEFINITION>	*m_Class;
	std::map<string,RPG_BAGTYPE>			*m_Bagtype;
};


/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__RPG_XML_DATALOADER_H__