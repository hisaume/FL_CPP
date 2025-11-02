#ifndef __FL_XML_STRINGTABLE_H__
#define __FL_XML_STRINGTABLE_H__
/*-----------------------------------------------------------------
|	|FILE|:		FL_XML_StringTable.h
|
|	FL Library
|	version 2.12
|------------------------------------------------------------------
	|NOTES|:

	Expected XML format by example:

	<flxml formatVersion="1.0">
		<stringtable author="FLRPGTeam">
			<string id="genericmenu_graphics"	display="Graphics" />
			<string id="genericmenu_sound"		display="Sound" />
			<string id="genericmenu_controls"	display="Controls" />
			<string id="genericmenu_language"	display="Language" />
			<string id="genericmenu_exit"		display="Exit" />
			<string id="genericmenu_translator"	display="Translation by:" />
		</stringtable>
	</flxml>
-----------------------------------------------------------------*/
#include <string>
#include <map>

using namespace std;


// Forward declarations
class TiXmlElement;


class StringTableLoaderXML
{
public:
	StringTableLoaderXML() : m_StringTableHandle( NULL )
	{}
	virtual ~StringTableLoaderXML() {}

	void Parse( const std::string& filename, std::map<string,wstring> *stringTableHandle );

protected:
	void ProcessStringTable( TiXmlElement *XMLRoot );

	void ProcessStringLine( TiXmlElement *XMLNode );

	string getAttrib(TiXmlElement *XMLNode, const string &parameter, const string &defaultValue = "");
	float getAttribReal(TiXmlElement *XMLNode, const string &parameter, float defaultValue = 0);
	int getAttribInt(TiXmlElement *XMLNode, const string &parameter, int defaultValue = 0);
	bool getAttribBool(TiXmlElement *XMLNode, const string &parameter, bool defaultValue = false);

protected:
	std::map<string,wstring> *m_StringTableHandle;
};

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__FL_XML_STRINGTABLE_H__