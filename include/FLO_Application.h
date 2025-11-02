#ifndef __FLG_APPLICATION_H__
#define __FLG_APPLICATION_H__
/*---------------------------------------------------------------------------
|	|FILE|:    FLG_Application.h
|
|	FL Library
|	version 2.11
|----------------------------------------------------------------------------
	|NOTES|:

	FL_Application is the base class to any application/game
	and isn't designed to be used alone. Derived class must implement
	SetupApplication() as the point of entry for a specific application.

---------------------------------------------------------------------------*/
#include <memory>
#include <Ogre.h>

#include <FL_General.h>
#include <FL_OISInput.h>
#include <FLO_Font.h>
#include <FLO_TerrainTools.h>


using namespace Ogre;


/*--------------------------------------
	Config file reader encapsulation

	config file content:

	[AnyConfigSectionName]
	left_side_var = right_side_value
	# any comment

	TODO: Need Unicode support
---------------------------------------*/
class FL_OgreConfigFile
{
public:
	FL_OgreConfigFile( const std::string& config_filename )
	{	
		ConfigFile cf;
		cf.load( config_filename );

		String secTitle;
		ConfigFile::SectionIterator seci = cf.getSectionIterator();
		while (seci.hasMoreElements())
		{
			secTitle = seci.peekNextKey();
			ConfigFile::SettingsMultiMap *settings = seci.getNext();
			ConfigFile::SettingsMultiMap::iterator i;
			for( i = settings->begin(); i != settings->end(); ++i )
			{
				// Search for the secTitle within existing list
				for( std::deque<CONFIGFILE_SECTION>::iterator j=m_SectionList.begin(); j!=m_SectionList.end(); ++j ) {
					if( j->SectionTitle == secTitle ) {
						j->LeftRight[i->first] = i->second;
						break;
					}
				}
				// section title not found. make a new one.
				CONFIGFILE_SECTION section;
				section.SectionTitle = secTitle;
				section.LeftRight[i->first] = i->second;
				m_SectionList.push_back( section );
			}
		}
	}

public:
	struct CONFIGFILE_SECTION {
		String SectionTitle;					// [...]
		std::map<String,String>	LeftRight;		// LeftSide = RightSide
	};
	std::deque<CONFIGFILE_SECTION>	m_SectionList;
};


/*--------------------------------------------------------------------------------------
	Ogre Application class contains basic Ogre environment.
	It is derived from FrameListener to provide frameStarted() method off Ogre,
	and derived from FL_BufferedInput to allow user input.

	Derive from this to use Ogre with relative ease.

	In the child class, overrite frameStarted and input listeners from FL_BufferedInput
	to add functionality.

	Basic use:

		main()
		{
			FL_Application app;

			try {
				app.go();
			}
			catch( Exception& e ) {
				// handle exception.
			}
		}

	Derived class must implement SetupApplication() for its own initialization.

	To use m_Joypad buffered input, make sure to call FL_BufferedInput::keyPressed line of methods
	from FL_BufferedInput base class!
*/

#ifdef _DEBUG
#define FLAPP_PLUGINS_FILENAME	"plugins_d.cfg"
#else
#define FLAPP_PLUGINS_FILENAME	"plugins.cfg"
#endif

//#define FLAPP_CONFIG_FILENAME	"ogre.cfg"
//#define FLAPP_LOG_FILENAME		"ogre.log"

class FL_Application 
	: public WindowEventListener, public FrameListener, public FL_BufferedInput
{
public:
	FL_Application(	const std::string& ogreCfgNameNoExtension ="graphics",
					const std::string& configFilesPath =".\\", const std::string& logFilePath =".\\" )
		:	m_MainScene(NULL),	m_Viewport(NULL),	m_bExit(false), m_bTerrainLoaded( false )
			, m_FontMain(NULL)
			, m_InputManager(NULL), m_Keyboard(NULL), m_Mouse(NULL), m_Joystick(NULL)
	{
		m_PLUGINS_CFG_Path		= configFilesPath;
		m_RESOURCES_CFG_Path	= configFilesPath;
		m_OGRE_CFG_Path			= configFilesPath;
		m_OGRE_CFG_Name			= ogreCfgNameNoExtension + ".cfg";
		m_OGRE_LOG_Path			= logFilePath;
		m_OGRE_LOG_Name			= ogreCfgNameNoExtension + ".log";
	}
	virtual ~FL_Application()
	{
		DestroyScene();
		// Destroy overlay
		//OverlayManager::getSingleton().destroy( m_Overlay );

		LOG( L"Releasing input system... " );
        m_InputManager->destroyInputObject(m_Keyboard); LOG(L"Kebyoard... ");
		m_InputManager->destroyInputObject(m_Mouse); LOG(L"Mouse... ");
		m_InputManager->destroyInputObject(m_Joystick); LOG(L"Joystick... ");
        OIS::InputManager::destroyInputSystem(m_InputManager); LOG(L"Input manager... ");
		LOG( L"Done.\n" );
//		CEGUI::OgreRenderer::destroySystem();
//		delete m_ExitListener;
		m_Root.reset(); LOG(L"Root reset for release & FL application ending.\n");
		LOG(   L"End of application destructor.\n" );
	}

	void Go()
	{
		CreateRoot();
		DefineResources();
		ConfigRenderSystem();
		DetectHardwareCapability();
		InitializeResourceGroups();
		SetupInputSystem();
		CreateFrameListener();
		//CreateOverlay();
		FL_Application::CreateFont();
		LoadScene( "" );

		StartApplication();

		StartRenderLoop();
	}

protected:
	/*-------------------
		Setup methods
	-------------------*/
    void CreateRoot()
	{
		//m_Root = new Root();
		LOG( L"Starting Root... " );
		m_Root.reset( new Root( m_PLUGINS_CFG_Path + FLAPP_PLUGINS_FILENAME,
								m_OGRE_CFG_Path + m_OGRE_CFG_Name,
								m_OGRE_LOG_Path + m_OGRE_LOG_Name ) );	// auto_ptr reset
		LOG( L"Done.\n" );
	}
    
	virtual void DefineResources()
	{
		LOG( L"Define resources... " );

		//----- Original Ogre method : eventually comment out -----//
		String secName, typeName, archName;
		ConfigFile cf;
		cf.load( m_RESOURCES_CFG_Path + "resources.cfg" );

		ConfigFile::SectionIterator seci = cf.getSectionIterator();
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			ConfigFile::SettingsMultiMap *settings = seci.getNext();
			ConfigFile::SettingsMultiMap::iterator i;
			for( i = settings->begin(); i != settings->end(); ++i )
			{
				typeName = i->first;
				archName = i->second;
				// Register resource path
				ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			}
		}
		LOG( L"Done.\n" );
	}

	bool ConfigRenderSystem()
    {
		LOG( L"Configuring render system... " );
		if( !m_Root->restoreConfig() && !m_Root->showConfigDialog() ) {
			LOG( L"User cancelled.\n" );
			throw Exception(52, "User canceled the config dialog!", "FL_Application::configRenderSystem()");
			return false;
		}
		else {
			LOG( L"initializing render window... " );
			m_RenderWindow = m_Root->initialise( true, "OGRE" );	// create render window and retain handle
			LOG( L"Done.\n" );
			return true;
		}
		/*-------------------------------------------------------------------------------------------------------
			You can use the Root::getAvailableRenderers to find out which RenderSystems are available for
			your application to use. Once you have retrieved a RenderSystem, you can use the
			RenderSystem::getConfigOptions to see what options are available for the user.
			By combining these two function calls, you can create your own config dialog for your application.
	        
			RenderSystem *rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
												// or use "OpenGL Rendering Subsystem"
			mRoot->setRenderSystem(rs);
			rs->setConfigOption("Full Screen", "No");
			rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
		-------------------------------------------------------------------------------------------------------*/
	}

	void DetectHardwareCapability()
	{
	 	if (m_Root->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
			m_bInfiniteFarPlane = true;
		else	m_bInfiniteFarPlane = false;
	}

    void InitializeResourceGroups()
	{
		LOG( L"Initializing resource groups... " );
		LOG( L"texture manager... " );
        TextureManager::getSingleton().setDefaultNumMipmaps( 5 );
		LOG( L"resource group manager... " );
        ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		LOG( L"Done.\n" );
	}

	void SetupInputSystem()
	{
		LOG( L"Initializing input system... " );

		//----- Setup InputManager -----
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		OIS::ParamList pl;
		RenderWindow *win = m_RenderWindow;//m_Root->getAutoCreatedWindow();

		LOG( L"Input manager... " );

		win->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		m_InputManager = OIS::InputManager::createInputSystem(pl);

		LOG( L"Done.\n" );

		//-----  -----
		try
		{
			LOG( L"   Keyboard... " );
			m_Keyboard = static_cast<OIS::Keyboard*>( m_InputManager->createInputObject(OIS::OISKeyboard, true));

		} catch( const OIS::Exception &e ) {
			LOG( L"None.\n" );
			throw Exception(42, e.eText, "FL_Application::setupInputSystem");
		}
		LOG( L"Done.\n" );

		try
		{
			LOG( L"   Mouse... " );
			m_Mouse = static_cast<OIS::Mouse*>( m_InputManager->createInputObject(OIS::OISMouse, true) );

		} catch( OIS::Exception ) {
			LOG( L"(None) " );
			m_Mouse = NULL;
		}
		LOG( L"Done.\n" );

		try {
			LOG( L"   Joystick... " );
			m_Joystick = static_cast<OIS::JoyStick*>( m_InputManager->createInputObject(OIS::OISJoyStick, true) );

		} catch( OIS::Exception ) {
			LOG( L"(None) " );
			m_Joystick = NULL;
		}
		LOG( L"Done.\n" );

		CreateBufferedInput( m_Keyboard, m_Mouse, m_Joystick );

		LOG( L"Input systems done.\n" );
	}

	void CreateFrameListener()
	{
		/*	Listener for ESC to quit.
			Comment out the following 2 lines once input is properly handled
			by deriving from input listeners off FL_BufferedInput class		*/
		//m_ExitListener = new ExitListener(m_Keyboard);
		//m_Root->addFrameListener(m_ExitListener);

		// Register itself to make use of frameStarted method
		m_Root->addFrameListener( (FL_Application*)this );
		// Window listener
		WindowEventUtilities::addWindowEventListener( m_RenderWindow, this );
	}

	/*
	void CreateOverlay()
	{
		OverlayManager& om = OverlayManager::getSingleton();
		m_Overlay = om.create( String("FLO_Overlay") );
		m_Overlay->show();
	}*/

	/*	Not all applications want the same default font.
		Override this in a derived class if needed. */
	virtual void CreateFont()
	{
		m_FontMain = new FL_OgreFont( "font_main", "M+1P+IPAG.ttf", true, J_SMALL, 18, 96 );
		m_FontMain->SetHeight( 0.04f );
	}

	virtual void StartApplication()		{}

	void StartRenderLoop()
	{
		LOG( L"Entering render loop.\n" );
		m_Root->startRendering();
	}

	/*----------------------
		Frame Listener
	----------------------*/
	// Call this explicitly in the derived class's frameStarted().
	virtual bool frameStarted( const FrameEvent& evt )
	{
		return true;
	}
    virtual bool frameRenderingQueued( const Ogre::FrameEvent& evt )
	{
		if( m_bExit )
			return false;

		if( m_Keyboard ) {
			m_Keyboard->capture();
			// keys to Quit program
			if( m_Keyboard->isKeyDown(OIS::KC_ESCAPE)
				|| ( m_Keyboard->isKeyDown(OIS::KC_LMENU) && m_Keyboard->isKeyDown(OIS::KC_F4) ) )
				return false;
		}
		if( m_Mouse )
			m_Mouse->capture();
		if( m_Joystick )
			m_Joystick->capture();

		m_Joypad.FrameStarted( evt.timeSinceLastFrame );	// button press duration update

		return true;
	}

	void windowClosed( RenderWindow* rw )
	{
		m_bExit = true;
	}

protected:
	// LoadScene("") will load default scene manager with nothing in it.
	virtual bool LoadScene( const std::string& dotscene_file )
	{
		assert( m_MainScene==NULL );

		m_MainScene = new FL_SCENEPACK();

		if( dotscene_file.empty() )	{
			CreateEmptyScene( m_MainScene );	// create scene manager without terrain
			m_bTerrainLoaded = false;
		}
		else {
			CreateScene_DotScene( m_MainScene, dotscene_file );
			m_bTerrainLoaded = true;
		}

		// Attach viewport to the camera created in the scene
		m_Viewport = m_RenderWindow->addViewport( m_MainScene->m_Camera );	// create viewport for the entire screen, for the m_Camera
		// Match m_Camera's aspect ratio to that of viewport
		m_MainScene->m_Camera->setAspectRatio( Real(m_Viewport->getActualWidth()) / Real(m_Viewport->getActualHeight()) );
		// Set initial camera conditions
		SetCameraProperties_Typical( m_MainScene->m_Camera, m_bInfiniteFarPlane );
	//	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	//	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

		return true;
	}
	void UnloadScene()
	{
		// TODO: Destroy specific elements in the scene without destroying the scene manager.
		// For now, just destroy the entire scene manager object
		DestroyScene();
		m_bTerrainLoaded = false;
	}

private:
	virtual void DestroyScene()
	{
		LOG( L"Destroying scene manager... " );
		if( m_MainScene )
			SAFE_DELETE( m_MainScene );
		if ( m_Viewport != NULL ) m_RenderWindow->removeViewport( m_Viewport->getZOrder() );
		m_Viewport = NULL;
		m_bTerrainLoaded = false;
		LOG( L"Done.\n" );
	}


public:
	/*------------------------------------------
		Misc Access Methods for Convenience
	------------------------------------------*/
	// Returns Vector3::ZERO if not found.
/*	Vector3 GetPinPosition( unsigned int idx )	{
													if( idx < m_PinIndex.size() )	return m_PinIndex[idx];
													else	return Vector3::ZERO;
												}
	// Returns -1 if not found.
	int GetEventIDFromEntityName( String meshName )	{	std::map<String,unsigned int>::iterator it;
														it = m_EventMesh.find( meshName );
														if( it==m_EventMesh.end() )
															return -1;
														return it->second;
													}*/

protected:
	/*	Graphics	*/
	std::auto_ptr<Root> m_Root;
	Ogre::RenderWindow	*m_RenderWindow;
	Ogre::Viewport		*m_Viewport;
	// Scene manager structure
	FL_SCENEPACK		*m_MainScene;
	// Hardware Capabilities flag
	bool m_bInfiniteFarPlane;

	/*	Main overlay	*/
	//Ogre::Overlay		*m_Overlay;		// top most overlay

	/*	Font support */
	FL_OgreFont			*m_FontMain;

	/*	Input		*/
	OIS::InputManager	*m_InputManager;
    OIS::Keyboard		*m_Keyboard;
	OIS::Mouse			*m_Mouse;
	OIS::JoyStick		*m_Joystick;

	std::string			m_PLUGINS_CFG_Path;
	std::string			m_RESOURCES_CFG_Path;
	std::string			m_OGRE_CFG_Path;
	std::string			m_OGRE_CFG_Name;
	std::string			m_OGRE_LOG_Path;
	std::string			m_OGRE_LOG_Name;

//	CEGUI::OgreRenderer *m_CEGUIRenderer;
//	ExitListener		*m_ExitListener;

	bool				m_bExit;
	bool				m_bTerrainLoaded;		// whether main terrain is loaded
	//--------------------------------------------------

};

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif
