#include <FLO_TerrainTools.h>
#include <FLO_TerrainLoaderXML.h>
#include <FL_General.h>


void CreateEmptyScene( FL_SCENEPACK *sp )
{
	LOG( L"Creating scene manager without terrain... " );

	// Create scene manager
	sp->m_SceneManager = Root::getSingleton().createSceneManager( ST_GENERIC, "DefaultSceneManager" );
	sp->m_SceneManager->setAmbientLight( ColourValue(1.0f,1.0f,1.0f) );	// Set general lighting
	sp->m_Camera = sp->m_SceneManager->createCamera("Camera");		// Create m_Camera

	LOG( L"Done.\n" );
}


void CreateScene_DotScene( FL_SCENEPACK *sp ,const string& dotscene_file )
{
	LOG( L"Creating scene manager with terrain... " );

	// Create scene manager
	sp->m_SceneManager = Root::getSingleton().createSceneManager(ST_GENERIC, "DefaultSceneManager");
	sp->m_SceneManager->setAmbientLight( ColourValue(1.0f,1.0f,1.0f) );		// Set general lighting
	sp->m_Camera = sp->m_SceneManager->createCamera("Camera");		// Create m_Camera

	sp->m_BuildingNode = sp->m_SceneManager->getRootSceneNode()->createChildSceneNode( "BuildingNode" );
	sp->m_WalkPlatformNode = sp->m_SceneManager->getRootSceneNode()->createChildSceneNode( "WalkPlatformNode" );

	LOG( L"Done.\n" );
	LOG( L"Loading DS... ");

	FL_TERRAININFO tInfo;

	/*			TODO:	Insert tf into DotSceneLoader and have it filled by parser.
						Otherwise the tf info doesn't exist.
	*/



/*	!!!!!!!!!!!!!!!!!!!!!!!!!   temporarily comment out to build !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		DO NOT forget to Uncomment this later

	std::auto_ptr<DotSceneLoader> sceneLoader(new DotSceneLoader());
	sceneLoader->parseDotScene( dotscene_file, "General", sp );

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/





	// Send FL_SCENEPACK and FL_TERRAININFO to construct terrain
	bool construct_terrain = ConstructTerrain( sp, &tInfo );
	assert( construct_terrain == true );

	// copy life and event mesh data
	//sp->m_LifeSetup = tInfo.m_LifeArray;
	//sp->m_EventMesh = tInfo.m_EventMesh;
	// copy pin index
	//sp->m_PinIndex.clear();
	//sp->m_PinIndex.resize( tInfo.m_PinIndex.size() );
	//for( unsigned int i=0; i<tInfo.m_PinIndex.size(); ++i )
	//	sp->m_PinIndex[i] = tInfo.m_PinIndex[i].GetVector3();

	LOG( L"Done.\n" );

	// PnP TerrainCreator flips the Z. Correct this.
	//SceneNode *sn = m_SceneManager->getRootSceneNode();
	//Ogre::Node::ChildNodeIterator iter = sn->getChildIterator();
	//while( iter.hasMoreElements() ) {
	//	Ogre::Node *node = iter.getNext();
	//	Vector3 pos = node->getPosition();
	//	node->translate( 0, 0, m_PageWorldZ-pos.z );
	//}
}


void SetCameraProperties_Typical( Ogre::Camera *cam, bool bInfiniteFarPlane )
{
	cam->setPosition( Ogre::Vector3(-200, 370, 650) );
	cam->lookAt( Ogre::Vector3(-200, 0, 300) );
	cam->setNearClipDistance( 1.0 );
	if( bInfiniteFarPlane )
		cam->setFarClipDistance( 0 );   // enable infinite far clip distance if we can
	else
		cam->setFarClipDistance( 2000 );
}


bool ConstructTerrain( FL_SCENEPACK *sp, FL_TERRAININFO *tInfo )
{
	//----- Terrain Group creation -----
	sp->m_TerrainGroup = OGRE_NEW Ogre::TerrainGroup(	sp->m_SceneManager, Ogre::Terrain::ALIGN_X_Z,
											tInfo->m_TerrainSize, (Ogre::Real)tInfo->m_WorldSize );
	sp->m_TerrainGroup->setOrigin(Ogre::Vector3::ZERO);


	// light used by TerrainGlobalOptions to create terrain DAT file, if needed
	Ogre::Vector3 lightdir(0.4f, -0.3f, -0.4f);
	lightdir.normalise();
	Ogre::Light* light = sp->m_SceneManager->createLight( "tstLight" );
	light->setType( Ogre::Light::LT_DIRECTIONAL );
	light->setDirection( lightdir );
	light->setDiffuseColour( Ogre::ColourValue::White );
	light->setSpecularColour( Ogre::ColourValue(0.1f, 0.1f, 0.1f) );

	//----- Terrain Global Options creation and init ------
	//Important to set these so that the terrain knows what to use for derived (non-realtime) data
	sp->m_TerrainGlobalOptions = OGRE_NEW Ogre::TerrainGlobalOptions();

	//------ ************ Hardware compatibility. Looks less pretty with the following code.
	/*
	TerrainMaterialGeneratorA::SM2Profile* matProfile =
	   static_cast<TerrainMaterialGeneratorA::SM2Profile*>(sp->m_TerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setLightmapEnabled(false);
	matProfile->setLayerNormalMappingEnabled(false);
	matProfile->setLayerParallaxMappingEnabled(false);
	matProfile->setReceiveDynamicShadowsEnabled(false);
	matProfile->setReceiveDynamicShadowsLowLod(false);
	*/
	//------ ****************************************************************** end above.

	sp->m_TerrainGlobalOptions->setMaxPixelError( (Ogre::Real)tInfo->m_MaxPixelError );
	// composite map distance is the distance of the light map which is to be created by ogre
	sp->m_TerrainGlobalOptions->setCompositeMapDistance( 3000 );
	sp->m_TerrainGlobalOptions->setLightMapDirection( light->getDerivedDirection() );
	sp->m_TerrainGlobalOptions->setCompositeMapAmbient( sp->m_SceneManager->getAmbientLight() );
	sp->m_TerrainGlobalOptions->setCompositeMapDiffuse( light->getDiffuseColour() );

	//----- Import Setting ----- Configure default import settings for if we use imported image (raw file)
	Ogre::Terrain::ImportData& defaultimp = sp->m_TerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = tInfo->m_TerrainSize;	// 1025, most likely
	defaultimp.worldSize = (Ogre::Real)tInfo->m_WorldSize;		// 1024, most likely
	// ???????????????????????????
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
    
	// Set textures to be loaded per layer
	int num_Layers = 0;
	for( unsigned int i=0; i<tInfo->m_BlendImagePNG.size(); ++i )
		if( !tInfo->m_BlendImagePNG[i].empty() )
			++num_Layers;	// count up num layers defined in XML
	defaultimp.layerList.resize(num_Layers);
	for( int i=0; i<num_Layers; ++i ) {
		defaultimp.layerList[i].worldSize = (Ogre::Real)tInfo->m_TileSize; // size of each tile in world frame of reference
		defaultimp.layerList[i].textureNames.push_back( tInfo->m_BlendImagePNG[i]+"_diffusespecular.dds" );
		defaultimp.layerList[i].textureNames.push_back( tInfo->m_BlendImagePNG[i]+"_normalheight.dds" );
	}

	//----- Define Terrain page by page -----
	sp->m_TerrainLoadedFromRAW = false;		// assume terrain DAT file already exists
	sp->m_TerrainGroup->setFilenameConvention( tInfo->m_TerrainImageRAW_NoExtension, Ogre::String("dat") );
//		for (unsigned int x = 0; x < sceneLoader.m_TerrainPagesX; ++x)
	for (unsigned int x = 0; x < OGT_PageX; ++x)
	{
//			for (unsigned int z = 0; z < sceneLoader.m_TerrainPagesZ; ++z)
		for (unsigned int z = 0; z < OGT_PageY; ++z)
		{
			/* DO NOT generate filename because the filename is stored directly in the XML. Use that name. */
			//Ogre::String filename = m_TerrainGroup->generateFilename(x, z);// construct terrain data filename
			Ogre::String filename = tInfo->m_OGTFilename[z][x];
			// check if the terrain data file already exists on disc
			if (Ogre::ResourceGroupManager::getSingleton().resourceExists(sp->m_TerrainGroup->getResourceGroup(), filename))
				sp->m_TerrainGroup->defineTerrain(x, z); // loading from constructed DAT file
			else {
				// terrain DATA doesn't exist yet, so construct it by loading RAW heightmap
				Ogre::DataStreamPtr stream1 = Ogre::ResourceGroupManager::getSingleton().openResource( "testfield1.raw", "General" );
				size_t size1 = stream1.get()->size();
				if(size1 != 1025 * 1025 * 4)
				{
					OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR, "Size of stream does not match terrainsize!", "TerrainPage" );
				}
				float* buffer1 = OGRE_ALLOC_T(float, size1, Ogre::MEMCATEGORY_GEOMETRY);
				stream1->read(buffer1, size1);
				Ogre::Terrain::ImportData& imp = sp->m_TerrainGroup->getDefaultImportSettings();
				imp.terrainSize = tInfo->m_TerrainSize;	// likely 1025;
				imp.worldSize = (Ogre::Real)tInfo->m_WorldSize;		// likely 1024;
				imp.inputFloat = buffer1;
				imp.inputImage = 0;
				imp.deleteInputData = true;
				imp.minBatchSize = 33;
				imp.maxBatchSize = 65;
				sp->m_TerrainGroup->defineTerrain(x, z, &imp);

				// Always put this flag up after loading from a file
				sp->m_TerrainLoadedFromRAW = true;
			}
		}
	}
	sp->m_TerrainGroup->loadAllTerrains(true);
 
	if( sp->m_TerrainLoadedFromRAW )
	{
		// If terrain is loaded from heightmap, it needs texture layers created on top of it.
		// So iterate through each terrain page and apply textures.
		Ogre::TerrainGroup::TerrainIterator ti = sp->m_TerrainGroup->getTerrainIterator();
		int x=0, z=0;	// 1st & 2nd part of "_00000_00000_000"
		while( ti.hasMoreElements() ) // Page Iteration
		{
			Ogre::Terrain* terrain = ti.getNext()->instance;
			//----- Texturing by alpha splattering ------
			// alpha blend maps for alpha splattering
			std::vector<Ogre::String> blendImages;
			blendImages.resize(num_Layers);
			// cook 00000 bit of "_00000_00000_000" in PNG filename
			char cbufferX[6], cbufferZ[6];
			sprintf( cbufferX, "%05d", x );
			sprintf( cbufferZ, "%05d", z );
			for( int i=0; i<num_Layers; ++i )
				blendImages[i] = tInfo->m_BlendImagePNG[i]
								+ "_" + cbufferX + "_" + cbufferZ + "_000.PNG";
				//blendImages[0] = "grass3_00000_00000_000.PNG"; <-- supposed outcome

			// load those blendmaps into the layers
			for(int j = 1;j < terrain->getLayerCount();j++)
			{
				Ogre::TerrainLayerBlendMap *blendmap = terrain->getLayerBlendMap(j);
				Ogre::Image img;
				img.load(blendImages[j],"General");
				int blendmapsize = terrain->getLayerBlendMapSize();
				if(img.getWidth() != blendmapsize)
					img.resize(blendmapsize, blendmapsize);

				float *ptr = blendmap->getBlendPointer();
				Ogre::uint8 *data = static_cast<Ogre::uint8*>(img.getPixelBox().data);

				for(int bp = 0;bp < blendmapsize * blendmapsize;bp++)
					ptr[bp] = static_cast<float>(data[bp]) / 255.0f;

				blendmap->dirty();
				blendmap->update();
			}
			//----- Update x & z numbering for the filename -----
			++x;
//				if( x >= (int)sceneLoader.m_TerrainPagesX ) {
			if( x >= (int)OGT_PageX ) {
				/*
					ASSUMPTION WARNING:
						x & Z processing assumes that page iteration begins at bottom left page
				*/
				x=0;
				++z;
			}
		}
		sp->m_TerrainGroup->saveAllTerrains(true);
	}
	
	sp->m_TerrainGroup->freeTemporaryResources();

	return true;
}

/*
	Used by RaycastFromPoint() above.
*/
void GetMeshInformation(const Entity *entity,
								size_t &vertex_count,
								Ogre::Vector3* &vertices,
								size_t &index_count,
								unsigned long* &indices,
								const Ogre::Vector3 &position,
								const Ogre::Quaternion &orient,
								const Ogre::Vector3 &scale)
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;
	vertex_count = index_count = 0;

	Ogre::MeshPtr mesh = entity->getMesh();


	bool useSoftwareBlendingVertices = entity->hasSkeleton();

	if (useSoftwareBlendingVertices)
	{
		((Ogre::Entity*)entity)->_updateAnimation();
	}

	// Calculate how many vertices and indices we're going to need
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh( i );

		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}

		// Add the indices
		index_count += submesh->indexData->indexCount;
	}


	// Allocate space for the vertices and indices
	vertices = new Ogre::Vector3[vertex_count];
	indices = new unsigned long[index_count];

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

	  //----------------------------------------------------------------
	  // GET VERTEXDATA
	  //----------------------------------------------------------------

		//Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
	  Ogre::VertexData* vertex_data;

	  //When there is animation:
	  if(useSoftwareBlendingVertices)
	#ifdef BUILD_AGAINST_AZATHOTH
		 vertex_data = submesh->useSharedVertices ? entity->_getSharedBlendedVertexData() : entity->getSubEntity(i)->_getBlendedVertexData();
	#else
		 vertex_data = submesh->useSharedVertices ? entity->_getSkelAnimVertexData() : entity->getSubEntity(i)->_getSkelAnimVertexData();
	#endif
	  else
		 vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;


		if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			Ogre::HardwareVertexBufferSharedPtr vbuf =
				vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex =
				static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;

			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);

				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

				vertices[current_offset + j] = (orient * (pt * scale)) + position;
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}


		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
		size_t index_start = index_data->indexStart;
		size_t last_index = numTris*3 + index_start;

		if (use32bitindexes)
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[index_offset++] = pLong[k] + static_cast<unsigned long>( offset );
			}

		else
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[ index_offset++ ] = static_cast<unsigned long>( pShort[k] ) +
					static_cast<unsigned long>( offset );
			}

		ibuf->unlock();
		current_offset = next_offset;
	}
}


// raycast from a point in to the scene.
// returns success or failure.
// on success the point is returned in the result.
bool RaycastFromPoint( Ogre::SceneManager *sceneManager, const Vector3 &point, const Vector3 &normal,
						Vector3 &result, Ogre::uint32 mask =0, Ogre::Entity *entityHandle =NULL )
															/*	Potential corruption. Change to **entityHandle if it is. */
{
	// create the ray scene query object
	Ogre::RaySceneQuery *m_pray_scene_query = sceneManager->createRayQuery( Ogre::Ray(),
												Ogre::SceneManager::USER_TYPE_MASK_LIMIT );
												//Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK );

	if (NULL == m_pray_scene_query)
	{
		ERR( L"Failed to create Ogre::RaySceneQuery instance.\n" );
		return (false);
	}
	m_pray_scene_query->setSortByDistance(true);
	if( mask != 0 )
		m_pray_scene_query->setQueryMask( mask );			// target building/platform only

	// create the ray to test
	Ogre::Ray ray(Ogre::Vector3(point.x, point.y, point.z),
				  Ogre::Vector3(normal.x, normal.y, normal.z));

	// check we are initialised
	if (m_pray_scene_query != NULL)
	{
		// create a query object
		m_pray_scene_query->setRay(ray);
 
		// execute the query, returns a vector of hits
		if (m_pray_scene_query->execute().size() <= 0)
		{
			sceneManager->destroyQuery( m_pray_scene_query );
			// raycast did not hit an objects bounding box
			return (false);
		}
	}
	else
	{
		sceneManager->destroyQuery( m_pray_scene_query );
		ERR( L"Cannot raycast without RaySceneQuery instance.\n" );
		return (false);
	}   
 
	// at this point we have raycast to a series of different objects bounding boxes.
	// we need to test these different objects to see which is the first polygon hit.
	// there are some minor optimizations (distance based) that mean we wont have to
	// check all of the objects most of the time, but the worst case scenario is that
	// we need to test every triangle of every object.
	Ogre::Real closest_distance = -1.0f;
	Ogre::Vector3 closest_result;
	Ogre::RaySceneQueryResult &query_result = m_pray_scene_query->getLastResults();
	for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
	{
		// stop checking if we have found a raycast hit that is closer
		// than all remaining entities
		if ((closest_distance >= 0.0f) &&
			(closest_distance < query_result[qr_idx].distance))
		{
			 break;
		}
 
		// only check this result if its a hit against an entity
		if ((query_result[qr_idx].movable != NULL) &&
			(query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
		{
			// get the entity to check
			Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);           
 
			// mesh data to retrieve         
			size_t vertex_count;
			size_t index_count;
			Ogre::Vector3 *vertices;
			unsigned long *indices;
 
			// get the mesh information
			GetMeshInformation(	pentity, vertex_count, vertices, index_count, indices,
								pentity->getParentNode()->_getDerivedPosition(),
								pentity->getParentNode()->_getDerivedOrientation(),
								pentity->getParentNode()->_getDerivedScale() );
 
			// test for hitting individual triangles on the mesh
			bool new_closest_found = false;
			for (int i = 0; i < static_cast<int>(index_count); i += 3)
			{
				// check for a hit against this triangle
				std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
					vertices[indices[i+1]], vertices[indices[i+2]], true, false);
 
				// if it was a hit check if its the closest
				if (hit.first)
				{
					if ((closest_distance < 0.0f) ||
						(hit.second < closest_distance))
					{
						// this is the closest so far, save it off
						closest_distance = hit.second;
						new_closest_found = true;
						//----- save custom info from entity -----
						if( entityHandle )
							entityHandle = pentity;	// save entity address
					}
				}
			}
 
		 // free the verticies and indicies memory
			delete[] vertices;
			delete[] indices;
 
			// if we found a new closest raycast for this object, update the
			// closest_result before moving on to the next object.
			if (new_closest_found)
			{
				closest_result = ray.getPoint(closest_distance);               
			}
		}       
	}

	sceneManager->destroyQuery( m_pray_scene_query );

	// return the result
	if (closest_distance >= 0.0f)
	{
		// raycast success
		result = closest_result;
		return (true);
	}
	else
	{
		// raycast failed
		return (false);
	} 
}
