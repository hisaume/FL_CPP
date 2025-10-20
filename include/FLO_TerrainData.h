#ifndef __FL_TERRAININFO_H__
#define __FL_TERRAININFO_H__
/*-----------------------------------------------------------------
|	|FILE|:    FL_TerrainInfo.h
|
|	FL Library
|	version 2.11

-----------------------------------------------------------------*/

#include <string>
#include <Ogre.h>	// Only needed for Vector3 & String
#include <OgreTerrain.h>
#include <OgreTerrainGroup.h>

using namespace Ogre;

// Terrain page dimension. See m_OGTFilename[y][x] member.
const int OGT_PageX = 1, OGT_PageY = 1;


class Vector3X : public Vector3
{
public:
	Vector3X() : Vector3(0,0,0)	{}

	virtual Vector3X operator=( Vector3 &v )	{	x=v.x; y=v.y; z=v.z; return *this; }
	Vector3	GetVector3()	{ return (Vector3)(*this); }
};

struct DOTSCENE_NODEDATA
{
	bool	bEventTrigger;
	String	sMeshFilename;	// resource filename
	//----- User Data -----
	String	sRayquery_mask;
	String	sID;
	String	sDisplayName;
	String	sTemplate;
	String	sMesh;			// user assigned filename
	int		iCheckFlag;

	DOTSCENE_NODEDATA() : bEventTrigger(false), iCheckFlag(-1)	{}
	void InsertNODEDATA( DOTSCENE_NODEDATA &d )
	{
		bEventTrigger=d.bEventTrigger; sMeshFilename=d.sMeshFilename;
		sRayquery_mask=d.sRayquery_mask;
		sID=d.sID; sDisplayName=d.sDisplayName;
		sTemplate=d.sTemplate; sMesh=d.sMesh; iCheckFlag=d.iCheckFlag;
	}
};

struct DOTSCENE_LIFE : public DOTSCENE_NODEDATA
{
	Vector3		position;

	DOTSCENE_LIFE() : position(Vector3::ZERO)	{}
};

struct EVENT_ZONE
{
	unsigned int	ID;
	// coordinate variables here...
	Ogre::Box		box;
};


/*
	Terrain information & Nodes
*/
struct FL_TERRAININFO
{
	unsigned int mPageSize, mPageWorldX, mPageWorldZ;//, mMaxHeight;
	unsigned int m_TerrainSize, m_WorldSize, m_DetailTile, m_TileSize, m_MaxPixelError, m_MaxMipMapLevel;
	unsigned int m_TerrainPagesX, m_TerrainPagesZ;

	//----- Ogitor addition -----
	std::string m_OGTFilename[OGT_PageY][OGT_PageX];		// [y][x] page of the terrain paging.

	//----- former PnP addition -----
	std::string m_TerrainImageRAW, m_TerrainImageRAW_NoExtension;
	std::vector<String> m_BlendImagePNG;
	std::string m_DetailTexture;

	//std::vector<DOTSCENE_LIFE>		m_LifeArray;
	//std::map<String,unsigned int>	m_EventMesh;	// stores a pair made of Mesh Name & event ID.
	//std::vector<Vector3X>			m_PinIndex;		// array of Vector3 pins on the terrain with pin ID.
	//std::vector<EVENT_ZONE>			m_EventZone;	// array of cubic definition and event ID.

	FL_TERRAININFO() :	mPageSize(0), mPageWorldX(0), mPageWorldZ(0),
						m_TerrainSize(0), m_WorldSize(0), m_DetailTile(0), m_TileSize(0),
						m_MaxPixelError(0), m_MaxMipMapLevel(0), m_TerrainPagesX(0), m_TerrainPagesZ(0)
	{}
};


/*
	Scene structure

	Bundle together scene manager with its scene related objects
	such as camera and nodes, and create/destroy them together.
	These are all contained within the same scene / terrain.

	NOTE: Viewport must be managed by the caller and is not included.
			Viewport is application dependent and should not be
			associated only with a particular scene.
			Calling application should have a "Render Window" and
			"Viewport", and attach viewport to the camera created
			inside this structure as needed.
*/
struct FL_SCENEPACK : public FL_TERRAININFO
{
	Ogre::SceneManager			*m_SceneManager;
	Ogre::Camera				*m_Camera;
	Ogre::TerrainGlobalOptions	*m_TerrainGlobalOptions;
    Ogre::TerrainGroup			*m_TerrainGroup;

	Ogre::SceneNode				*m_WalkPlatformNode;	// Mesh on which character can walk
	Ogre::SceneNode				*m_BuildingNode;		// Parent node to structures/buildings
    bool						m_TerrainLoadedFromRAW;
	std::vector<DOTSCENE_LIFE>			m_LifeSetup;
	std::map<String,unsigned int>		m_EventMesh;
	std::vector<Vector3>				m_PinIndex;

	FL_SCENEPACK() :	m_SceneManager(NULL),	m_Camera(NULL),
						m_TerrainGlobalOptions(NULL),	m_TerrainGroup(NULL),
						m_WalkPlatformNode(NULL),	// Mesh on which character can walk
						m_BuildingNode(NULL),	// Parent node to structures/buildings
						m_TerrainLoadedFromRAW(false)
	{
		/* DO NOTHING HERE. DO NOT CREATE. */
	}

	~FL_SCENEPACK()
	{
		OGRE_DELETE m_TerrainGroup;
		m_TerrainGroup = NULL;
		OGRE_DELETE m_TerrainGlobalOptions;
		m_TerrainGlobalOptions = NULL;
		// NOTE: ** IS camera named "Camera" removed? **
		if( m_SceneManager != NULL ) Root::getSingleton().destroySceneManager( m_SceneManager );
		m_SceneManager = NULL;
		m_Camera = NULL;
	}
};


/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	// __FL_TERRAININFO_H__