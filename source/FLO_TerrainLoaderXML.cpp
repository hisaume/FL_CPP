#include <tinyxml.h>
#include <Ogre.h>

#include <FL_GeneralTools.h>	// for LONG2STRING and CHAR2LONG function
#include <FLO_TerrainLoaderXML.h>


using namespace std;
using namespace Ogre;


void DotSceneLoader::destroyVectors()
{
	nodeProperties.clear();
	staticObjects.clear();
	dynamicObjects.clear();
}

void DotSceneLoader::parseDotScene(const String &SceneName, const String &groupName, FL_SCENEPACK *sp,
								   SceneNode *pAttachNode, const String &sPrependNode)
{
	// set up shared object values
	scenepack = sp;
	mSceneMgr = scenepack->m_SceneManager;
	mBuildingNode = scenepack->m_BuildingNode;
	mPlatformNode = scenepack->m_WalkPlatformNode;
	m_sGroupName = groupName;
	m_sPrependNode = sPrependNode;
	staticObjects.clear();
	dynamicObjects.clear();

	TiXmlDocument   *XMLDoc = 0;
	TiXmlElement   *XMLRoot;

	try
	{
		// Strip the path
		Ogre::String basename, path;
		Ogre::StringUtil::splitFilename(SceneName, basename, path);

		DataStreamPtr pStream = ResourceGroupManager::getSingleton().
			openResource( basename, groupName );

		//DataStreamPtr pStream = ResourceGroupManager::getSingleton().
		//	openResource( SceneName, groupName );

		String data = pStream->getAsString();
		// Open the .scene File
		XMLDoc = new TiXmlDocument();
		XMLDoc->Parse( data.c_str() );
		pStream->close();
		pStream.setNull();

		if( XMLDoc->Error() )
		{
			//We'll just log, and continue on gracefully
			LogManager::getSingleton().logMessage("[DotSceneLoader] The TiXmlDocument reported an error");
			delete XMLDoc;
			return;
		}
	}
	catch(...)
	{
		//We'll just log, and continue on gracefully
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating TiXmlDocument");
		delete XMLDoc;
		return;
	}

	// Validate the File
	XMLRoot = XMLDoc->RootElement();
	if( String( XMLRoot->Value()) != "scene"  ) {
		LogManager::getSingleton().logMessage( "[DotSceneLoader] Error: Invalid .scene File. Missing <scene>" );
		delete XMLDoc;
		return;
	}

	// figure out where to attach any nodes we create
	mAttachNode = pAttachNode;
	if(!mAttachNode)
		mAttachNode = mSceneMgr->getRootSceneNode();

	// Process the scene
	processScene(XMLRoot);

	// Close the XML File
	delete XMLDoc;
}

void DotSceneLoader::processScene(TiXmlElement *XMLRoot)
{
	// Process the scene parameters
	String version = getAttrib(XMLRoot, "formatVersion", "unknown");

	String message = "[DotSceneLoader] Parsing dotScene file with version " + version;
	if(XMLRoot->Attribute("ID"))
		message += ", id " + String(XMLRoot->Attribute("ID"));
	if(XMLRoot->Attribute("sceneManager"))
		message += ", scene manager " + String(XMLRoot->Attribute("sceneManager"));
	if(XMLRoot->Attribute("minOgreVersion"))
		message += ", min. Ogre version " + String(XMLRoot->Attribute("minOgreVersion"));
	if(XMLRoot->Attribute("author"))
		message += ", author " + String(XMLRoot->Attribute("author"));

	LogManager::getSingleton().logMessage(message);

	TiXmlElement *pElement;

	// Process terrain (?)
	pElement = XMLRoot->FirstChildElement("terrain");
	if(pElement)
		processTerrain(pElement);

	// Process nodes (?)
	pElement = XMLRoot->FirstChildElement("nodes");
	if(pElement)
		processNodes(pElement);

	// Process externals (?)
	pElement = XMLRoot->FirstChildElement("externals");
	if(pElement)
		processExternals(pElement);

	// Process environment (?)
	pElement = XMLRoot->FirstChildElement("environment");
	if(pElement)
		processEnvironment(pElement);

	// Process userDataReference NOT USED IN THIS SECTION IN FLRPG
	/*
	pElement = XMLRoot->FirstChildElement("userDataReference");
	if(pElement)
		processUserDataReference(pElement);
	*/

	// Process octree (?)
	pElement = XMLRoot->FirstChildElement("octree");
	if(pElement)
		processOctree(pElement);

	// Process light (?)
	pElement = XMLRoot->FirstChildElement("light");
	if(pElement)
		processLight(pElement);

	// Process camera (?)
	pElement = XMLRoot->FirstChildElement("camera");
	if(pElement)
		processCamera(pElement);
}

void DotSceneLoader::processNodes(TiXmlElement *XMLNode)
{
	TiXmlElement *pElement;

	// Process node (*)
	pElement = XMLNode->FirstChildElement("node");
	while(pElement)
	{
		processNode(pElement);
		pElement = pElement->NextSiblingElement("node");
	}

	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
	{
		mAttachNode->setPosition(parseVector3(pElement));
		mAttachNode->setInitialState();
	}

	// Process rotation (?)
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
	{
		mAttachNode->setOrientation(parseQuaternion(pElement));
		mAttachNode->setInitialState();
	}
	else {
		pElement = XMLNode->FirstChildElement("quaternion");
		if(pElement)
		{
			mAttachNode->setOrientation(parseQuaternion(pElement));
			mAttachNode->setInitialState();
		}
	}

	// Process scale (?)
	pElement = XMLNode->FirstChildElement("scale");
	if(pElement)
	{
		mAttachNode->setScale(parseVector3(pElement));
		mAttachNode->setInitialState();
	}
}

/*
void DotSceneLoader::processUserData(TiXmlElement *XMLNode)
{
	TiXmlElement *pElement;

	// Process node (*)
	pElement = XMLNode->FirstChildElement("userData");
	while(pElement)
	{
		processNode(pElement);
		pElement = pElement->NextSiblingElement("userData");
	}
}*/

void DotSceneLoader::processExternals(TiXmlElement *XMLNode)
{
	//! @todo Implement this
}

void DotSceneLoader::processEnvironment(TiXmlElement *XMLNode)
{
	TiXmlElement *pElement;

	// Process fog (?)
	pElement = XMLNode->FirstChildElement("fog");
	if(pElement)
		processFog(pElement);

	// Process skyBox (?)
	pElement = XMLNode->FirstChildElement("skyBox");
	if(pElement)
		processSkyBox(pElement);

	// Process skyDome (?)
	pElement = XMLNode->FirstChildElement("skyDome");
	if(pElement)
		processSkyDome(pElement);

	// Process skyPlane (?)
	pElement = XMLNode->FirstChildElement("skyPlane");
	if(pElement)
		processSkyPlane(pElement);

	// Process clipping (?)
	pElement = XMLNode->FirstChildElement("clipping");
	if(pElement)
		processClipping(pElement);

	// Process colourAmbient (?)
	pElement = XMLNode->FirstChildElement("colourAmbient");
	if(pElement)
		mSceneMgr->setAmbientLight(parseColour(pElement));

	// Process colourBackground (?)
	//! @todo Set the background colour of all viewports (RenderWindow has to be provided then)
	pElement = XMLNode->FirstChildElement("colourBackground");
	if(pElement)
		;//mSceneMgr->set(parseColour(pElement));

	// Process userDataReference (?)
	/*
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
		processUserDataReference(pElement);
	*/
}

void DotSceneLoader::processTerrainPages(TiXmlElement *XMLNode)
{
	TiXmlElement *pElement;

	// Iterate through 'terrainPage' subsections
	pElement = XMLNode->FirstChildElement("terrainPage");
	while(pElement)
	{
		//-----	process content of this 'terrainPage' -----
		//	FORMAT:	<terrainPage name="Page_ffffffff.ogt" pageX="-1" pageY="-1" />
		int pageX = getAttribInt( pElement, "pageX", -100000 );	// set impossible value by default
		int pageY = getAttribInt( pElement, "pageY", -100000 );
		if( pageX==-100000 || pageY==-100000 ) {			// check they exist.
			String msg = "[DotSceneLoader] ERROR: pageX or pageY is missing in terrain information.";
			LogManager::getSingleton().logMessage(msg);
			continue;
		}
		if( pageX >= OGT_PageX || pageY >= OGT_PageY ) {	// boundary check.
			String msg = "[DotSceneLoader] ERROR: pageX or pageY exceeds allowed dimension.";
			LogManager::getSingleton().logMessage(msg);
			continue;
		}
		scenepack->m_OGTFilename[pageY][pageX] = getAttrib( pElement, "name" );

		//----- proceed to next terrainPage -----
		pElement = pElement->NextSiblingElement("terrainPage");
	}

}

void DotSceneLoader::processTerrain(TiXmlElement *XMLNode)
{
	TiXmlElement *pElement;

	/*----------------------
		Orgitor addition
	----------------------*/

	// process 'terrainPages' segment
	pElement = XMLNode->FirstChildElement("terrainPages");
	if( pElement )
		processTerrainPages( pElement );


	/*-------------------------
		Former PnP addition
	-------------------------*/
	String str = getAttrib(XMLNode, "TerrainImageRAW");
	if (str.size() != 0) {
		scenepack->m_TerrainImageRAW = str;
		int no_extension_length = scenepack->m_TerrainImageRAW.length()-4;	// minus ".RAW"
		char *cbuffer = new char[no_extension_length+1];
		memcpy( cbuffer, scenepack->m_TerrainImageRAW.c_str(), sizeof(char)*no_extension_length );
		cbuffer[no_extension_length] = '\0';
		scenepack->m_TerrainImageRAW_NoExtension = cbuffer;
		delete[] cbuffer;
	}

	//----- tile image names, also used for alpha splattering maps
	//		Limit to 4 textures for now
	scenepack->m_BlendImagePNG.resize(4);
	str = getAttrib(XMLNode, "BlendImagePNG1");
	if (str.size() != 0)
		scenepack->m_BlendImagePNG[0] = str;
	str = getAttrib(XMLNode, "BlendImagePNG2");
	if (str.size() != 0)
		scenepack->m_BlendImagePNG[1] = str;
	str = getAttrib(XMLNode, "BlendImagePNG3");
	if (str.size() != 0)
		scenepack->m_BlendImagePNG[2] = str;
	str = getAttrib(XMLNode, "BlendImagePNG4");
	if (str.size() != 0)
		scenepack->m_BlendImagePNG[3] = str;
	//--------------------------------------------------------------------

	str = getAttrib(XMLNode, "DetailTexture");
	if (str.size() != 0)
		scenepack->m_DetailTexture = str;

	str = getAttrib(XMLNode, "TerrainPagesX");
	if (str.size() != 0)
		scenepack->m_TerrainPagesX = scenepack->mPageSize = (unsigned int)FL::CHAR2LONG( str.c_str() );

	str = getAttrib(XMLNode, "TerrainPagesZ");
	if (str.size() != 0)
		scenepack->m_TerrainPagesZ = scenepack->mPageSize = (unsigned int)FL::CHAR2LONG( str.c_str() );

	str = getAttrib(XMLNode, "TerrainSize");
	if( str.empty() )
		str = getAttrib(XMLNode, "mapSize");
	if (str.size() != 0)
		scenepack->m_TerrainSize = scenepack->mPageSize = (unsigned int)FL::CHAR2LONG( str.c_str() );


	str = getAttrib(XMLNode, "WorldSize");
	if( str.empty() )
		str = getAttrib(XMLNode, "worldSize");
	if (str.size() != 0)
		scenepack->m_WorldSize = scenepack->mPageWorldX = scenepack->mPageWorldZ = (unsigned int)FL::CHAR2LONG( str.c_str() );

	String detailTile = getAttrib(XMLNode, "DetailTile");
	if (detailTile.size() != 0)
		scenepack->m_DetailTile = (unsigned int)FL::CHAR2LONG( detailTile.c_str() );

	String tileSize = getAttrib(XMLNode, "TileSize");
	if (tileSize.size() != 0)
		scenepack->m_TileSize = (unsigned int)FL::CHAR2LONG( tileSize.c_str() );

	String maxPixelError = getAttrib(XMLNode, "MaxPixelError");
	if( maxPixelError.empty() )
		maxPixelError = getAttrib(XMLNode, "tuningMaxPixelError");
	if (maxPixelError.size() != 0)
		scenepack->m_MaxPixelError = (unsigned int)FL::CHAR2LONG( maxPixelError.c_str() );

	String maxMipMapLevel = getAttrib(XMLNode, "MaxMipMapLevel");
	if (maxMipMapLevel.size() != 0)
		scenepack->m_MaxMipMapLevel = (unsigned int)FL::CHAR2LONG( maxMipMapLevel.c_str() );


	//----- Older keywords : Unused in FLRPG -----

	String vertexNormals = getAttrib(XMLNode, "VertexNormals");
	if (vertexNormals.size() != 0)
	{}

	String vertexColors = getAttrib(XMLNode, "VertexColors");
	if (vertexColors.size() != 0)
	{}

	String useTriStrips = getAttrib(XMLNode, "UseTriStrips");
	if (useTriStrips.size() != 0)
	{}

	String vertexProgramMorph = getAttrib(XMLNode, "VertexProgramMorph");
	if (vertexProgramMorph.size() != 0)
	{}

	String morphLODFactorParamName = getAttrib(XMLNode, "MorphLODFactorParamName");
	if (morphLODFactorParamName.size() != 0)
	{}

	String morphLODFactorParamIndex = getAttrib(XMLNode, "MorphLODFactorParamIndex");
	if (morphLODFactorParamIndex.size() != 0)
	{}

	String lodMorphStart = getAttrib(XMLNode, "LODMorphStart");
	if (lodMorphStart.size() != 0)
	{}
	/*
		String worldTexture = getAttrib(XMLNode, "WorldTexture");
	if (worldTexture.size() != 0)
	{}
	String detailTexture = getAttrib(XMLNode, "DetailTexture");
	if (detailTexture.size() != 0)
	{}
	String pageSource = getAttrib(XMLNode, "PageSource");
	if (pageSource.size() != 0)
	{}
	String heightmapImage = getAttrib(XMLNode, "Heightmap.image");
	if (heightmapImage.size() != 0)
	{}
	String heightmapRawSize = getAttrib(XMLNode, "Heightmap.raw.size");
	if (heightmapRawSize.size() != 0)
	{}
	String heightmapRawBpp = getAttrib(XMLNode, "Heightmap.raw.bpp");
	if (heightmapRawBpp.size() != 0)
	{}
	String pageSize = getAttrib(XMLNode, "PageSize");
	if (pageSize.size() != 0)
	{}
	String pageWorldX = getAttrib(XMLNode, "PageWorldX");
	if (pageWorldX.size() != 0)
	{}
	String pageWorldZ = getAttrib(XMLNode, "PageWorldZ");
	if (pageWorldZ.size() != 0)
	{}
	String maxHeight = getAttrib(XMLNode, "MaxHeight");
	if (maxHeight.size() != 0)
	{}
	*/
}

/*	In FLRPG there is no chance parent node becomes necessary to set user data.
void DotSceneLoader::processUserDataReference(TiXmlElement *XMLNode, SceneNode *pParent, Entity *pEntity)
{
	TiXmlElement *pElement;

	pElement = XMLNode->FirstChildElement("property");
	while(pElement)
	{
		processProperty(pElement,pEntity);
		pElement = pElement->NextSiblingElement("property");
	}
}*/

void DotSceneLoader::processUserDataReference(TiXmlElement *XMLNode, DOTSCENE_NODEDATA &userData )
{
	/***********************************************************

	Potentially entered from <node>, <entity> or <scene> etc.
	For FLRPG it's from <entity>. See processEntity().

	***********************************************************/
	TiXmlElement *pElement;

	pElement = XMLNode->FirstChildElement("property");
	while(pElement)
	{
		processProperty(pElement,userData);
		pElement = pElement->NextSiblingElement("property");
	}
}

void DotSceneLoader::processProperty(TiXmlElement *XMLNode, DOTSCENE_NODEDATA &userData)
{
	/*********************************
		Get this user data property
	*/
	String name = getAttrib(XMLNode, "name");

	if( name == "ID" )
		userData.sID = getAttrib(XMLNode, "data");
	else if( name=="displayName" )
		userData.sDisplayName = getAttrib(XMLNode, "data" );
	else if( name == "Template" )
		userData.sTemplate = getAttribInt(XMLNode, "data", 0);
	else if( name == "Mesh" )
		userData.sMesh = getAttrib(XMLNode, "data");

	else if( name == "CheckFlag" )
		userData.iCheckFlag = getAttribInt(XMLNode, "data", -1);
	else if( name=="MeshType" )
		userData.sRayquery_mask = getAttrib(XMLNode, "data");
	else if( name == "EventTrigger" )
		userData.bEventTrigger = getAttribBool(XMLNode, "data", false);
	/*
		EXAMPLE:	IGNORE 'type'
            <property type="7" name="ID" data="1" />
            <property type="7" name="displayName" data="" />
            <property type="2" name="Template" data="0" />
            <property type="7" name="Mesh" data="" />
            <property type="2" name="CheckFlag" data="0" />
            <property type="7" name="MeshType" data="VEGETATION" />
            <property type="12" name="EventTrigger" data="true" />
	*/
}

void DotSceneLoader::processProperty(TiXmlElement *XMLNode, Entity *pEntity)
{
	String name = getAttrib(XMLNode, "name");

	if( name=="MeshType" && pEntity!=NULL )
	{
		String data = getAttrib(XMLNode, "data");
		if( data == "LIFE" )
			pEntity->setQueryFlags( 1<<1 );
		else if( data == "BUILDING" )
			pEntity->setQueryFlags( 1<<2 );
		else if( data == "VEGETATION" )
			pEntity->setQueryFlags( 1<<3 );
	}
}


void DotSceneLoader::processOctree(TiXmlElement *XMLNode)
{
	//! @todo Implement this
}

void DotSceneLoader::processLight(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");

	// Create the light
	Light *pLight = mSceneMgr->createLight(name);
	if(pParent)
		pParent->attachObject(pLight);

	String sValue = getAttrib(XMLNode, "type");
	if(sValue == "point")
		pLight->setType(Light::LT_POINT);
	else if(sValue == "directional")
		pLight->setType(Light::LT_DIRECTIONAL);
	else if(sValue == "spot")
		pLight->setType(Light::LT_SPOTLIGHT);
	else if(sValue == "radPoint")
		pLight->setType(Light::LT_POINT);

	pLight->setVisible(getAttribBool(XMLNode, "visible", true));
	pLight->setCastShadows(getAttribBool(XMLNode, "castShadows", true));

	TiXmlElement *pElement;

	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
		pLight->setPosition(parseVector3(pElement));

	// Process normal (?)
	pElement = XMLNode->FirstChildElement("normal");
	if(pElement)
		pLight->setDirection(parseVector3(pElement));

	// Process colourDiffuse (?)
	pElement = XMLNode->FirstChildElement("colourDiffuse");
	if(pElement)
		pLight->setDiffuseColour(parseColour(pElement));

	// Process colourSpecular (?)
	pElement = XMLNode->FirstChildElement("colourSpecular");
	if(pElement)
		pLight->setSpecularColour(parseColour(pElement));

	// Process lightRange (?)
	pElement = XMLNode->FirstChildElement("lightRange");
	if(pElement)
		processLightRange(pElement, pLight);

	// Process lightAttenuation (?)
	pElement = XMLNode->FirstChildElement("lightAttenuation");
	if(pElement)
		processLightAttenuation(pElement, pLight);

	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
		;//processUserDataReference(pElement, pLight);
}

void DotSceneLoader::processCamera(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
	Real fov = getAttribReal(XMLNode, "fov", 45);
	Real aspectRatio = (Real)getAttribReal(XMLNode, "aspectRatio", 1.3333f);
	String projectionType = getAttrib(XMLNode, "projectionType", "perspective");

	// Find an existing cmaera
	Camera *pCamera = mSceneMgr->getCamera(name);
	
	if (pCamera == NULL)
	{
		// Create the camera
		Camera *pCamera = mSceneMgr->createCamera(name);
		if(pParent)
			pParent->attachObject(pCamera);
	}

	// Set the field-of-view
	//! @todo Is this always in degrees?
	pCamera->setFOVy(Ogre::Degree(fov));

	// Set the aspect ratio
	pCamera->setAspectRatio(aspectRatio);

	// Set the projection type
	if(projectionType == "perspective")
		pCamera->setProjectionType(PT_PERSPECTIVE);
	else if(projectionType == "orthographic")
		pCamera->setProjectionType(PT_ORTHOGRAPHIC);


	TiXmlElement *pElement;

	// Process clipping (?)
	pElement = XMLNode->FirstChildElement("clipping");
	if(pElement)
	{
		Real nearDist = getAttribReal(pElement, "near");
		pCamera->setNearClipDistance(nearDist);

		Real farDist =  getAttribReal(pElement, "far");
		pCamera->setFarClipDistance(farDist);
	}

	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
		pCamera->setPosition(parseVector3(pElement));

	// Process rotation (?)
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
		pCamera->setOrientation(parseQuaternion(pElement));

	pElement = XMLNode->FirstChildElement("lookAt");
	if(pElement)
		pCamera->lookAt(parseVector3(pElement));

	// Process normal (?)
	pElement = XMLNode->FirstChildElement("normal");
	if(pElement)
		;//!< @todo What to do with this element?

	// Process lookTarget (?)
	pElement = XMLNode->FirstChildElement("lookTarget");
	if(pElement)
		;//!< @todo Implement the camera look target

	// Process trackTarget (?)
	pElement = XMLNode->FirstChildElement("trackTarget");
	if(pElement)
		;//!< @todo Implement the camera track target

	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
		;//!< @todo Implement the camera user data reference
}

void DotSceneLoader::processNode(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Construct the node's name
	String name = m_sPrependNode + getAttrib(XMLNode, "name");

	// Create the scene node
	SceneNode *pNode;
	if(name.empty())
	{
		// Let Ogre choose the name
		if(pParent)
			pNode = pParent->createChildSceneNode();
		else
			pNode = mAttachNode->createChildSceneNode();
	}
	else
	{
		// Provide the name
		if(pParent)
			pNode = pParent->createChildSceneNode(name);
		else
			pNode = mAttachNode->createChildSceneNode(name);
	}

	// Process other attributes
	String id = getAttrib(XMLNode, "id");
	bool isTarget = getAttribBool(XMLNode, "isTarget");

	TiXmlElement *pElement;

	// Process position (?)
	Vector3 pos = Vector3::ZERO;
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
	{
		pos = parseVector3(pElement);
		// for PnP TerrainCreator POTENTIAL TROUBLE ********************************
		// PnP offset coordinates
		/*
			COMMENT THIS OUT for Ogitor
		*/
//		pos.z -= mPageWorldZ / 2;
//		pos.x -= mPageWorldX / 2;
		// *************************************************************************
		pNode->setPosition( pos );
		pNode->setInitialState();
	}

	// Process rotation (?)
	pElement = XMLNode->FirstChildElement("rotation");
	if( pElement==NULL )
		pElement = XMLNode->FirstChildElement("quaternion");
	if(pElement)
	{
		pNode->setOrientation(parseQuaternion(pElement));
		pNode->setInitialState();
	}

	// Process scale (?)
	pElement = XMLNode->FirstChildElement("scale");
	if(pElement)
	{
		pNode->setScale(parseVector3(pElement));
		pNode->setInitialState();
	}

	// Process lookTarget (?)
	pElement = XMLNode->FirstChildElement("lookTarget");
	if(pElement)
		processLookTarget(pElement, pNode);

	// Process trackTarget (?)
	pElement = XMLNode->FirstChildElement("trackTarget");
	if(pElement)
		processTrackTarget(pElement, pNode);

	// Process node (*)
	pElement = XMLNode->FirstChildElement("node");
	while(pElement)
	{
		processNode(pElement, pNode);
		pElement = pElement->NextSiblingElement("node");
	}


	/**-------------------------------------
		Entity handle for user data later & initialize
	*/
	Entity *pEntity = NULL;
	DOTSCENE_NODEDATA userData;

	// Process entity (*)
	pElement = XMLNode->FirstChildElement("entity");
	while(pElement)
	{
		pEntity = processEntity(pElement, pNode, userData);
		pElement = pElement->NextSiblingElement("entity");
	}

	// Process light (*)
	pElement = XMLNode->FirstChildElement("light");
	while(pElement)
	{
		processLight(pElement, pNode);
		pElement = pElement->NextSiblingElement("light");
	}

	// Process camera (*)
	pElement = XMLNode->FirstChildElement("camera");
	while(pElement)
	{
		processCamera(pElement, pNode);
		pElement = pElement->NextSiblingElement("camera");
	}

	// Process particleSystem (*)
	pElement = XMLNode->FirstChildElement("particleSystem");
	while(pElement)
	{
		processParticleSystem(pElement, pNode);
		pElement = pElement->NextSiblingElement("particleSystem");
	}

	// Process billboardSet (*)
	pElement = XMLNode->FirstChildElement("billboardSet");
	while(pElement)
	{
		processBillboardSet(pElement, pNode);
		pElement = pElement->NextSiblingElement("billboardSet");
	}

	// Process plane (*)
	pElement = XMLNode->FirstChildElement("plane");
	while(pElement)
	{
		processPlane(pElement, pNode);
		pElement = pElement->NextSiblingElement("plane");
	}

	/****************************************************************
		WATCH : User component

		Gather userData in <node> segment
	****************************************************************/
/*
		THIS IS NOT USED in FLRPG. UserData processed under <entity>.

	pElement = XMLNode->FirstChildElement("userData");
	while(pElement)
	{
		processUserDataReference(pElement, userData);
		pElement = pElement->NextSiblingElement("userData");
	}
*/

	/*--------------------------------------

				Process UserData

	--------------------------------------*/
	//----- Query mask -----
	if( pEntity )
	{
		String data = userData.sRayquery_mask;
		if( data == "LIFE" )
			pEntity->setQueryFlags( 1<<1 );
		else if( data == "BUILDING" )
			pEntity->setQueryFlags( 1<<2 );
		else if( data == "VEGETATION" )
			pEntity->setQueryFlags( 1<<3 );
		else
			pEntity->setQueryFlags( 1<<3 );	// default to 'RAYQUERY_VEGETATION'
	}

	/*
		Depending on mesh, some user data processing is necessary
	*/
	if( userData.sMeshFilename == "Life.mesh" )
	{
		// Note that pEntity is NULL in this case.
		DOTSCENE_LIFE	life;
		life.InsertNODEDATA( userData );
		life.position = pos;
		// Record this life for creation later
		scenepack->m_LifeSetup.push_back( life );
	}
	else if( userData.sMeshFilename == "Pin.mesh" )
	{
		// Note that pEntity is NULL in this case.
		if( !userData.sID.empty() ) {	// If PIN was used just as a marker, ID is empty. Ignore it then.
			unsigned int id = (int)FL::CHAR2LONG(userData.sID.c_str());	// convert ID to int
			if( id < scenepack->m_PinIndex.size() )
				scenepack->m_PinIndex[id] = pos;
			else {
				scenepack->m_PinIndex.resize(id+1);
				scenepack->m_PinIndex[id] = pos;
			}
		}
	}
/*	else if( userData.sMeshFilename == "EventZone.mesh" )
	{
		// event zone
		EVENT_ZONE eZone;
		eZone.ID = FL::CHAR2LONG(userData.sID.c_str());
		//	Set coordinate of the box
	
		float size = 2.0f;
		Ogre::Box b;
		b.bottom	= 0;
		b.top		= size;
		b.right		= size/2;
		b.left		= -(size/2);
		b.front 	= size/2;
		b.back		= -(size/2);
		pEntity->getParentNode()->getScale
		m_EventZone.push_back();
	}*/
	else {
		// Check for EVENT meshes. They can be of EventZone.mesh, or any other mesh.
		// If EventZone.mesh, it's invisible and event trigger occurs when player overlaps it.
		// If an other mesh, player can't (usually) overlap it but hitting action button at it triggers an event.
		if( userData.bEventTrigger && pEntity )
		{
			// Store in a map of key 'meshName' as used by Ogre
			if( FL::IsNumber(userData.sID) )
			{
				scenepack->m_EventMesh[pEntity->getName()] = FL::CHAR2LONG(userData.sID.c_str());
				if( userData.bEventTrigger )
					pEntity->addQueryFlags( 1<<0 );	// note it's 'add' because MeshType is already set.
				// special case of Event-trigger mesh which is invisible
				if( userData.sMeshFilename == "EventZone.mesh" )
					pEntity->setVisible( false );
			}
			else {
				String msg = String("[DotSceneLoader] Mesh [") +pEntity->getName()+ "] with event trigger has an invalid ID [" +userData.sID+ "].";
				LogManager::getSingleton().logMessage( msg );
			}
		}
	}
}

void DotSceneLoader::processLookTarget(TiXmlElement *XMLNode, SceneNode *pParent)
{
	//! @todo Is this correct? Cause I don't have a clue actually

	// Process attributes
	String nodeName = getAttrib(XMLNode, "nodeName");

	Node::TransformSpace relativeTo = Node::TS_PARENT;
	String sValue = getAttrib(XMLNode, "relativeTo");
	if(sValue == "local")
		relativeTo = Node::TS_LOCAL;
	else if(sValue == "parent")
		relativeTo = Node::TS_PARENT;
	else if(sValue == "world")
		relativeTo = Node::TS_WORLD;

	TiXmlElement *pElement;

	// Process position (?)
	Vector3 position;
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
		position = parseVector3(pElement);

	// Process localDirection (?)
	Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;
	pElement = XMLNode->FirstChildElement("localDirection");
	if(pElement)
		localDirection = parseVector3(pElement);

	// Setup the look target
	try
	{
		if(!nodeName.empty())
		{
			SceneNode *pLookNode = mSceneMgr->getSceneNode(nodeName);
			position = pLookNode->_getDerivedPosition();
		}

		pParent->lookAt(position, relativeTo, localDirection);
	}
	catch(Ogre::Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
	}
}

void DotSceneLoader::processTrackTarget(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String nodeName = getAttrib(XMLNode, "nodeName");

	TiXmlElement *pElement;

	// Process localDirection (?)
	Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;
	pElement = XMLNode->FirstChildElement("localDirection");
	if(pElement)
		localDirection = parseVector3(pElement);

	// Process offset (?)
	Vector3 offset = Vector3::ZERO;
	pElement = XMLNode->FirstChildElement("offset");
	if(pElement)
		offset = parseVector3(pElement);

	// Setup the track target
	try
	{
		SceneNode *pTrackNode = mSceneMgr->getSceneNode(nodeName);
		pParent->setAutoTracking(true, pTrackNode, localDirection, offset);
	}
	catch(Ogre::Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
	}
}

static unsigned int entity_idx = 0;

Entity* DotSceneLoader::processEntity(TiXmlElement *XMLNode, SceneNode *pParent, DOTSCENE_NODEDATA &userData)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");

	// Get mesh filename
	String meshFile = getAttrib(XMLNode, "meshFile");
	if( meshFile == String("") )
		meshFile = getAttrib(XMLNode, "meshfile");	// seek lower case as well (PnP TerrainCreator issue)
	userData.sMeshFilename = meshFile;

	TiXmlElement *pElement;

	// Process userDataReference
	pElement = XMLNode->FirstChildElement("userData");
	if(pElement)
		processUserDataReference(pElement, userData);

	//---------------------------------------------------------------------
	// Bail out if it's a life or mesh. They are for setup purposes only.
	if( meshFile=="Life.mesh" || meshFile=="Pin.mesh" )
		return NULL;


	String materialFile = getAttrib(XMLNode, "materialFile");
	bool isStatic = getAttribBool(XMLNode, "static", false);;
	bool castShadows = getAttribBool(XMLNode, "castShadows", true);

	// TEMP: Maintain a list of static and dynamic objects
	if(isStatic)
		staticObjects.push_back(name);
	else
		dynamicObjects.push_back(name);

	// Process vertexBuffer (?)
	pElement = XMLNode->FirstChildElement("vertexBuffer");
	if(pElement)
		;//processVertexBuffer(pElement);

	// Process indexBuffer (?)
	pElement = XMLNode->FirstChildElement("indexBuffer");
	if(pElement)
		;//processIndexBuffer(pElement);


/******************************************************************************


		** WATCH **	ENTITY and mesh file


******************************************************************************/

	// Create the entity
	Entity *pEntity = 0;
	try
	{
		MeshManager::getSingleton().load(meshFile, m_sGroupName);
		if( name.empty() )
			// Let Ogre choose the name
			pEntity = mSceneMgr->createEntity(meshFile);
		else {
			// Provide the name. In case the same name is used, add an incremental number.
			name += FL::LONG2STRING( entity_idx++ );
			pEntity = mSceneMgr->createEntity(name, meshFile);
			/*-------------------------------------------------------
			TODO:	If additional data needs polling and associating
					with this meshfile, provide a structure and fill
					it here.
			*/
		}

		pEntity->setCastShadows(castShadows);


		/*---------------------------------------------------------------------------------
		***********************************************************************************

		FOR NOW THE IDEA BELOW IS ABANDONED.

			Add the entity to parent node.
			NOTE:	Choose correct node here!! (Root node, Building node, Platform node)

		[mPlatformNode]
			Detect "WALK_" at the start of the mesh name?
		[mBuildingNode]
			Detect "STRUCT_" at the start of the mesh name?

		---------------------------------------------------------------------------------*/

		pParent->attachObject(pEntity);

		if(!materialFile.empty())
			pEntity->setMaterialName(materialFile);
	}
	catch(Ogre::Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
	}

	return pEntity;
}

void DotSceneLoader::processParticleSystem(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
	String file = getAttrib(XMLNode, "file");

	// Create the particle system
	try
	{
		ParticleSystem *pParticles = mSceneMgr->createParticleSystem(name, file);
		pParent->attachObject(pParticles);
	}
	catch(Ogre::Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
	}
}

void DotSceneLoader::processBillboardSet(TiXmlElement *XMLNode, SceneNode *pParent)
{
	//! @todo Implement this
}

void DotSceneLoader::processPlane(TiXmlElement *XMLNode, SceneNode *pParent)
{
	//! @todo Implement this
}

void DotSceneLoader::processFog(TiXmlElement *XMLNode)
{
	// Process attributes
	Real expDensity = getAttribReal(XMLNode, "expDensity", 0.001f);
	Real linearStart = getAttribReal(XMLNode, "linearStart", 0.0f);
	Real linearEnd = getAttribReal(XMLNode, "linearEnd", 1.0f);

	FogMode mode = FOG_NONE;
	String sMode = getAttrib(XMLNode, "mode");
	if(sMode == "none")
		mode = FOG_NONE;
	else if(sMode == "exp")
		mode = FOG_EXP;
	else if(sMode == "exp2")
		mode = FOG_EXP2;
	else if(sMode == "linear")
		mode = FOG_LINEAR;

	TiXmlElement *pElement;

	// Process colourDiffuse (?)
	ColourValue colourDiffuse = ColourValue::White;
	pElement = XMLNode->FirstChildElement("colourDiffuse");
	if(pElement)
		colourDiffuse = parseColour(pElement);

	// Setup the fog
	mSceneMgr->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
}

void DotSceneLoader::processSkyBox(TiXmlElement *XMLNode)
{
	// Process attributes
	String material = getAttrib(XMLNode, "material");
	Real distance = getAttribReal(XMLNode, "distance", 5000);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

	TiXmlElement *pElement;

	// Process rotation (?)
	Quaternion rotation = Quaternion::IDENTITY;
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
		rotation = parseQuaternion(pElement);

	// Setup the sky box
	mSceneMgr->setSkyBox(true, material, distance, drawFirst, rotation, m_sGroupName);
}

void DotSceneLoader::processSkyDome(TiXmlElement *XMLNode)
{
	// Process attributes
	String material = XMLNode->Attribute("material");
	Real curvature = getAttribReal(XMLNode, "curvature", 10);
	Real tiling = getAttribReal(XMLNode, "tiling", 8);
	Real distance = getAttribReal(XMLNode, "distance", 4000);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

	TiXmlElement *pElement;

	// Process rotation (?)
	Quaternion rotation = Quaternion::IDENTITY;
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
		rotation = parseQuaternion(pElement);

	// Setup the sky dome
	mSceneMgr->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, m_sGroupName);
}

void DotSceneLoader::processSkyPlane(TiXmlElement *XMLNode)
{
	// Process attributes
	String material = getAttrib(XMLNode, "material");
	Real planeX = getAttribReal(XMLNode, "planeX", 0);
	Real planeY = getAttribReal(XMLNode, "planeY", -1);
	Real planeZ = getAttribReal(XMLNode, "planeX", 0);
	Real planeD = getAttribReal(XMLNode, "planeD", 5000);
	Real scale = getAttribReal(XMLNode, "scale", 1000);
	Real bow = getAttribReal(XMLNode, "bow", 0);
	Real tiling = getAttribReal(XMLNode, "tiling", 10);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

	// Setup the sky plane
	Plane plane;
	plane.normal = Vector3(planeX, planeY, planeZ);
	plane.d = planeD;
	mSceneMgr->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, m_sGroupName);
}

void DotSceneLoader::processClipping(TiXmlElement *XMLNode)
{
	//! @todo Implement this

	// Process attributes
	Real fNear = getAttribReal(XMLNode, "near", 0);
	Real fFar = getAttribReal(XMLNode, "far", 1);
}

void DotSceneLoader::processLightRange(TiXmlElement *XMLNode, Light *pLight)
{
	// Process attributes
	Real inner = getAttribReal(XMLNode, "inner");
	Real outer = getAttribReal(XMLNode, "outer");
	Real falloff = getAttribReal(XMLNode, "falloff", 1.0);

	// Setup the light range
	pLight->setSpotlightRange(Angle(inner), Angle(outer), falloff);
}

void DotSceneLoader::processLightAttenuation(TiXmlElement *XMLNode, Light *pLight)
{
	// Process attributes
	Real range = getAttribReal(XMLNode, "range");
	Real constant = getAttribReal(XMLNode, "constant");
	Real linear = getAttribReal(XMLNode, "linear");
	Real quadratic = getAttribReal(XMLNode, "quadratic");

	// Setup the light attenuation
	pLight->setAttenuation(range, constant, linear, quadratic);
}


String DotSceneLoader::getAttrib(TiXmlElement *XMLNode, const String &attrib, const String &defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return XMLNode->Attribute(attrib.c_str());
	else
		return defaultValue;
}

Real DotSceneLoader::getAttribReal(TiXmlElement *XMLNode, const String &attrib, Real defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return StringConverter::parseReal(XMLNode->Attribute(attrib.c_str()));
	else
		return defaultValue;
}

int DotSceneLoader::getAttribInt(TiXmlElement *XMLNode, const String &attrib, int defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return StringConverter::parseInt(XMLNode->Attribute(attrib.c_str()));
	else
		return defaultValue;
}

bool DotSceneLoader::getAttribBool(TiXmlElement *XMLNode, const String &attrib, bool defaultValue)
{
	if(!XMLNode->Attribute(attrib.c_str()))
		return defaultValue;

	if(String(XMLNode->Attribute(attrib.c_str())) == "true")
		return true;

	return false;
}


Vector3 DotSceneLoader::parseVector3(TiXmlElement *XMLNode)
{
	return Vector3(
		StringConverter::parseReal(XMLNode->Attribute("x")),
		StringConverter::parseReal(XMLNode->Attribute("y")),
		StringConverter::parseReal(XMLNode->Attribute("z"))
	);
}

Quaternion DotSceneLoader::parseQuaternion(TiXmlElement *XMLNode)
{
	//! @todo Fix this crap!

	Quaternion orientation;

	if(XMLNode->Attribute("qx"))
	{
		orientation.x = StringConverter::parseReal(XMLNode->Attribute("qx"));
		orientation.y = StringConverter::parseReal(XMLNode->Attribute("qy"));
		orientation.z = StringConverter::parseReal(XMLNode->Attribute("qz"));
		orientation.w = StringConverter::parseReal(XMLNode->Attribute("qw"));
	}
	else if(XMLNode->Attribute("axisX"))
	{
		Vector3 axis;
		axis.x = StringConverter::parseReal(XMLNode->Attribute("axisX"));
		axis.y = StringConverter::parseReal(XMLNode->Attribute("axisY"));
		axis.z = StringConverter::parseReal(XMLNode->Attribute("axisZ"));
		Real angle = StringConverter::parseReal(XMLNode->Attribute("angle"));;
		orientation.FromAngleAxis(Ogre::Angle(angle), axis);
	}
	else if(XMLNode->Attribute("angleX"))
	{
		Vector3 axis;
		axis.x = StringConverter::parseReal(XMLNode->Attribute("angleX"));
		axis.y = StringConverter::parseReal(XMLNode->Attribute("angleY"));
		axis.z = StringConverter::parseReal(XMLNode->Attribute("angleZ"));
		//orientation.FromAxes(&axis);
		//orientation.F
	}

	return orientation;
}

ColourValue DotSceneLoader::parseColour(TiXmlElement *XMLNode)
{
	return ColourValue(
		StringConverter::parseReal(XMLNode->Attribute("r")),
		StringConverter::parseReal(XMLNode->Attribute("g")),
		StringConverter::parseReal(XMLNode->Attribute("b")),
		XMLNode->Attribute("a") != NULL ? StringConverter::parseReal(XMLNode->Attribute("a")) : 1
	);
}

String DotSceneLoader::getProperty(const String &ndNm, const String &prop)
{
	for ( unsigned int i = 0 ; i < nodeProperties.size(); i++ )
	{
		if ( nodeProperties[i].nodeName == ndNm && nodeProperties[i].propertyNm == prop )
		{
			return nodeProperties[i].valueName;
		}
	}

	return "";
}

void DotSceneLoader::processUserDataReference(TiXmlElement *XMLNode, Entity *pEntity)
{
	String str = XMLNode->Attribute("id");
	pEntity->setUserAny(Any(str));
}
