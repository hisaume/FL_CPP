#ifndef __FLO_TERRAINTOOLS_H__
#define __FLO_TERRAINTOOLS_H__
/*-----------------------------------------------------------------
|	|FILE|:		FLO_TerrainTools.h
|
|	FL Library
|	version 2.12

-----------------------------------------------------------------*/
#include <string>
#include <Ogre.h>
#include <OgreTerrain.h>
#include <OgreTerrainGroup.h>
#include <OgreTerrainMaterialGeneratorA.h>

#include <FLO_TerrainData.h>


/* Create scene manager without loading terrain. */
void CreateEmptyScene( FL_SCENEPACK *sp );
/* Create scene manager with terrain, from dotscene file. */
void CreateScene_DotScene( FL_SCENEPACK *sp, const std::string& dotscene_file );
/* Constructs terrain in a given scene manager, using info in FL_TERRAININFO. */
bool ConstructTerrain( FL_SCENEPACK *sp, FL_TERRAININFO *tInfo );
/* Sets camera properties typically seen as a default. For convenience only. */
void SetCameraProperties_Typical( Ogre::Camera *cam, bool bInfiniteFarPlane =false );


/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__FLO_TERRAINTOOLS_H__
