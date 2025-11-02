#include <tinyxml.h>

#include <FL_General.h>
#include <FL_GeneralTools.h>	// for LONG2STRING and CHAR2LONG function
#include <FL_File.h>
#include <FL_XML_StringTable.h>


using namespace std;


void StringTableLoaderXML::Parse( const string& filename, map<string,wstring> *stringTableHandle )
{
	m_StringTableHandle = stringTableHandle;

	TiXmlDocument   *XMLDoc = 0;
	TiXmlElement   *XMLRoot;

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
			ERR( L"[StringTableLoaderXML] " );
			ERR( FL::ASCII_to_UTF8(XMLDoc->ErrorDesc()) );
			ERR( L" ROW" );
			LOGVAR( XMLDoc->ErrorRow() );
			ERR( L" COL" );
			LOGVAR( XMLDoc->ErrorCol(), true );
			delete XMLDoc;
			return;
		}
	}
	catch(...)
	{
		//We'll just log, and continue on gracefully
		ERR( L"[StringTableLoaderXML] Error creating TiXmlDocument" );
		delete XMLDoc;
		return;
	}

	// Validate the File
	XMLRoot = XMLDoc->RootElement();
	if( std::string(XMLRoot->Value()) != "flxml"  ) {
		ERR( L"[StringTableLoaderXML] Error: Invalid string table file. Missing <flxml>.\n" );
		delete XMLDoc;
		return;
	}
	LOG( L"[StringTableLoaderXML] Parsing in progress.\n" );

	const string version = getAttrib(XMLRoot, "formatVersion", "unknown");

	TiXmlElement *pElement;

	// Process the string table
	pElement = XMLRoot->FirstChildElement( "stringtable" );
	const string author = getAttrib(XMLRoot, "author", "unknown");
	while( pElement )
	{
		ProcessStringTable( pElement );
		pElement = pElement->NextSiblingElement( "stringtable" );
	}

	// Close the XML File
	delete XMLDoc;
}

void StringTableLoaderXML::ProcessStringTable( TiXmlElement *XMLRoot )
{
	TiXmlElement *pElement;

	pElement = XMLRoot->FirstChildElement( "string" );
	while( pElement )
	{
		ProcessStringLine( pElement );
		pElement = pElement->NextSiblingElement( "string" );
	}
}

void StringTableLoaderXML::ProcessStringLine(TiXmlElement *XMLNode)
{
	const string id = getAttrib(XMLNode, "id");
	const string display = getAttrib(XMLNode, "display");

	// record the id-display pair.
	if( !id.empty() )
		(*m_StringTableHandle)[id] = FL::ASCII_to_UTF8( display );
}

string StringTableLoaderXML::getAttrib(TiXmlElement *XMLNode, const string &attrib, const string &defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return XMLNode->Attribute(attrib.c_str());
	else
		return defaultValue;
}

float StringTableLoaderXML::getAttribReal(TiXmlElement *XMLNode, const string &attrib, float defaultValue)
{
	/*
	if(XMLNode->Attribute(attrib.c_str()))
		return StringConverter::parseReal(XMLNode->Attribute(attrib.c_str()));
	else
		return defaultValue;
		*/
	return 0;
}

int StringTableLoaderXML::getAttribInt(TiXmlElement *XMLNode, const string &attrib, int defaultValue)
{
	/*
	if(XMLNode->Attribute(attrib.c_str()))
		return StringConverter::parseInt(XMLNode->Attribute(attrib.c_str()));
	else
		return defaultValue;
		*/
	return 0;
}

bool StringTableLoaderXML::getAttribBool(TiXmlElement *XMLNode, const string &attrib, bool defaultValue)
{
	if(!XMLNode->Attribute(attrib.c_str()))
		return defaultValue;

	if(string(XMLNode->Attribute(attrib.c_str())) == "true")
		return true;

	return false;
}
