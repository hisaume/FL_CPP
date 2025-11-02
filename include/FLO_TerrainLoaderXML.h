#ifndef __FLO_TERRAINLOADERXML_H__
#define __FLO_TERRAINLOADERXML_H__
/*-----------------------------------------------------------------
|	|FILE|:		FLO_TerrainLoaderXML.h
|
|	FL Library
|	version 2.12

-----------------------------------------------------------------*/
#include <OgreString.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <vector>

#include <FLO_TerrainData.h>


// Forward declarations
class TiXmlElement;

namespace Ogre
{
	// Forward declarations
	class SceneManager;
	class SceneNode;

	class nodeProperty
	{
	public:
		String nodeName;
		String propertyNm;
		String valueName;
		String typeName;

		nodeProperty(const String &node, const String &propertyName, const String &value, const String &type)
			: nodeName(node), propertyNm(propertyName), valueName(value), typeName(type) {}
	};

	class DotSceneLoader
	{
	public:
		DotSceneLoader() : mSceneMgr(0)
		{}
		virtual ~DotSceneLoader() {}

		void parseDotScene(const String &SceneName, const String &groupName, FL_SCENEPACK *sp, SceneNode *pAttachNode = NULL, const String &sPrependNode = "");
		String getProperty(const String &ndNm, const String &prop) const;

		// Clears out the vectors storing strings.
		void destroyVectors();

	protected:
		void processScene(TiXmlElement *XMLRoot);

		void processNodes(TiXmlElement *XMLNode);
		void processExternals(TiXmlElement *XMLNode);
		void processEnvironment(TiXmlElement *XMLNode);
		void processTerrainPages(TiXmlElement *XMLNode);
		void processTerrain(TiXmlElement *XMLNode);
		void processOctree(TiXmlElement *XMLNode);
		void processLight(TiXmlElement *XMLNode, SceneNode *pParent = 0);
		void processCamera(TiXmlElement *XMLNode, SceneNode *pParent = 0);
//		void processUserDataReference(TiXmlElement *XMLNode, SceneNode *pParent = 0, Entity *pEntity=0 );
		void processUserDataReference(TiXmlElement *XMLNode, Entity *pEntity);
		void processUserDataReference(TiXmlElement *XMLNode, DOTSCENE_NODEDATA &userData );

		void processNode(TiXmlElement *XMLNode, SceneNode *pParent = 0);
//		void processUserData(TiXmlElement *XMLNode, SceneNode *pParent = 0);
		void processLookTarget(TiXmlElement *XMLNode, SceneNode *pParent);
		void processTrackTarget(TiXmlElement *XMLNode, SceneNode *pParent);
		Entity* processEntity(TiXmlElement *XMLNode, SceneNode *pParent, DOTSCENE_NODEDATA &userData);
		void processParticleSystem(TiXmlElement *XMLNode, SceneNode *pParent);
		void processBillboardSet(TiXmlElement *XMLNode, SceneNode *pParent);
		void processPlane(TiXmlElement *XMLNode, SceneNode *pParent);

		void processFog(TiXmlElement *XMLNode);
		void processSkyBox(TiXmlElement *XMLNode);
		void processSkyDome(TiXmlElement *XMLNode);
		void processSkyPlane(TiXmlElement *XMLNode);
		void processClipping(TiXmlElement *XMLNode);

		//----- User Data -----
		void processProperty(TiXmlElement *XMLNode, Entity *pEntity);
		void processProperty(TiXmlElement *XMLNode, DOTSCENE_NODEDATA &userData);

		void processLightRange(TiXmlElement *XMLNode, Light *pLight);
		void processLightAttenuation(TiXmlElement *XMLNode, Light *pLight);

		String getAttrib(TiXmlElement *XMLNode, const String &parameter, const String &defaultValue = "");
		Real getAttribReal(TiXmlElement *XMLNode, const String &parameter, Real defaultValue = 0);
		int getAttribInt(TiXmlElement *XMLNode, const String &parameter, int defaultValue = 0);
		bool getAttribBool(TiXmlElement *XMLNode, const String &parameter, bool defaultValue = false);

		Vector3 parseVector3(TiXmlElement *XMLNode);
		Quaternion parseQuaternion(TiXmlElement *XMLNode);
		ColourValue parseColour(TiXmlElement *XMLNode);


		SceneManager *mSceneMgr;
		SceneNode *mBuildingNode;
		SceneNode *mPlatformNode;

		SceneNode *mAttachNode;
		String m_sGroupName;
		String m_sPrependNode;

	protected:
		std::vector<nodeProperty> nodeProperties;
		std::vector<String> staticObjects;
		std::vector<String> dynamicObjects;

		FL_SCENEPACK *scenepack;	// handle to scene pack obj sent in from caller.
	};
}

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__FLO_TERRAINLOADERXML_H__