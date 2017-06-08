#include "Race.h"
#include "World.h"
#include "InputHandler.h"
#include "AIManager.h"
#include "MainListener.h"
#include "Camera.h"
#include "Player.h"
#include "HUD.h"
#include "Menu.h"
#include "Achievements.h"
#include <OIS/OIS.h>

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "Kinect_USF.h"
#include "Menu.h"
#include "Store.h"
#include "OgreOverlaySystem.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayElement.h"
#include "OgreFontManager.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "Sound.h"
#include "Logger.h"
#include "Ghost.h"
#include "JsonUtils.h"

#include "tinyxml.h"
#include "FileUtil.h"

#include <iostream>
#include <fstream>

Race::Race()
{
    mFrameListener = 0;
    mRoot = 0;
    // Provide a nice cross platform solution for locating the configuration files
    // On windows files are searched for in the current working directory, on OS X however
    // you must provide the full path, the helper function macBundlePath does this for us.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    mResourcePath = macBundlePath() + "/Contents/Resources/";
#else
    mResourcePath = "";
#endif
}

/// Standard destructor
Race::~Race()
{
    if (mFrameListener)
        delete mFrameListener;
    if (mRoot)
        delete mRoot;

}



void
Race::createCamera()
{
	mCamera = mSceneMgr->createCamera("Player1Cam");
	mCamera ->setPosition(Ogre::Vector3(0,10,3000));
	mCamera ->lookAt(Ogre::Vector3(0,0,0));

}



// We will create a single frame listener, to handle our main event loop.  While we could
// add as many frame listeners as we want (and let the main rendering loop call them),
// we would not have as much control over the order that the listeners are called --
//  so we will just have one listener and handle everything ourselves.
void 
Race::createFrameListener(void)
{
	mFrameListener = new MainListener(mWindow, mAIManager, mWorld, mRaceCamera, mKinect, mHUD, mAchievements, mPlayer);
	//mFrameListener = new MainListener(mWindow, mAIManager, mWorld, mRaceCamera, mGamepad, mPlayer, mHUD);
	mRoot->addFrameListener(mFrameListener);
	mFrameListener->setPaused(true);
	// mFrameListener->showDebugOverlay(true);

}

// We will have a single viewport.  If you wanted splitscreen, or a rear-view mirror, then
// you may want multiple viewports.
void 
Race::createViewports(void)
{
	    // Create one viewport, entire window


        Ogre::Viewport* vp = mWindow->addViewport(mCamera);
        vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
        // Alter the camera aspect ratio to match the viewport
        mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));    

}

// Here is where we set up all of the non-rendering stuff (our world, various managers, etc)
void 
Race::createScene() 
{
	//Ogre::FontPtr f = Ogre::FontManager::getSingleton().getByName("Big");
	//f->load();

	SoundBank::getInstance()->setup();
	mHUD = new HUD();
	mAchievements = new Achievements("Achievements.txt");
	mRaceCamera = new RaceCamera(mCamera);
	mWorld = new World(mSceneMgr, mHUD, mRaceCamera, this);
	mAIManager = new AIManager(mWorld,mSceneMgr);

	mWorld->AddAIManager(mAIManager);

	InputHandler::getInstance()->initialize(mWindow);
	InputHandler::getInstance()->setEventCallback(MenuManager::getInstance());

	mKinect = new Kinect();
	mKinect->initSensor();
	mPlayer = new Player(mWorld, mKinect, mAchievements);
	mWorld->AddPlayer(mPlayer);
	mRaceCamera->TrackObject(mPlayer);
//	Ogre::OverlayManager::getSingleton().getByName("Login/Failure")->show();

	mLogger =Logger::getInstance();

	//if (mLogger == 0)
	//{
	//Ogre::OverlayManager::getSingleton().getByName("ServerConnection/Failure")->hide();

	//}
	
	
	mKinect->addSkelListener(mLogger);
	mWorld->addPlayerListener(mLogger);

}
void
	Race::startGame(std::string raceType, bool doEdit)
{

	char header[500];
	bool timeLimit = raceType == "TargetRace";
	mWorld->StartGame(mCurrentLevel[raceType].c_str(),timeLimit, doEdit);

	if (raceType =="Race")
	{
	sprintf_s(header, 500, "\"subgame\":\"race\", \"level\":\"%s\"", (mCurrentLevel[raceType].c_str()));
	}
	else if (raceType == "Target")
	{
	sprintf_s(header, 500, "\"subgame\":\"target\", \"level\":\"%s\"", (mCurrentLevel[raceType].c_str()));
	}
	else
	{
	sprintf_s(header, 500, "\"subgame\":\"targetTimed\", \"level\":\"%s\"", (mCurrentLevel[raceType].c_str()));
	}
	mLogger->StartSession(header);
	mKinect->StartSession();
	mFrameListener->setPaused(false);
}

void Race::startRace()
{

	char buffer[500];


	sprintf_s(buffer, 500, "\"subgame\":\"race\"");

	mLogger->StartSession( buffer);
	mKinect->StartSession();
}


void Race::endGame()
{
	mLogger->EndSession();
	mKinect->EndSession();
	mFrameListener->setPaused(true);
	mWorld->destroyWorld();
	SoundBank::getInstance()->stopAllSounds();
}


void Race::writeConfigStr(int player)
{
	MenuManager *menus = MenuManager::getInstance();

	std::string result = getConfigString(player);
	//std::ofstream configFile;
	//configFile.open ("config.txt", std::ios::out);

//	configFile << result;
//	configFile.close();
	mLogger->sendProfileData(result);

}


void Race::setSingleConfig(std::string key, std::string value, int player)
{
	key = JSON_UTIL::stripQuotes(key);
	if (key == "menus")
	{
		MenuManager::getInstance()->setMenuConfig(value);
	}
	else if (key == "achievements")
	{
		mAchievements->setCompletedAchievements(value);
	}
	



}

void Race::setFromConfigString(std::string configString, int player)
{

	///TODO:  This parsing of JSON dictionaries really needs to be refactored ..

	std::size_t braceIndex = configString.find_first_of('{');
	if (braceIndex == std::string::npos)
	{
		return;
	}

	std::string remainder = configString.substr(braceIndex+1);

	std::size_t nextIndex = remainder.find_first_not_of("\t \n");
	while (nextIndex != std::string::npos && remainder[nextIndex] != '}')
	{
		std::string nextKey = JSON_UTIL::stripQuotes(JSON_UTIL::firstItem(remainder));

		remainder = JSON_UTIL::removeFirstitem(remainder);
	
		size_t colonIndex = remainder.find_first_of(":");
		if (colonIndex == std::string::npos)
		{
			// ERROR!
			nextIndex = std::string::npos;
			break;
		}
		remainder = remainder.substr(colonIndex+1);
		std::string value = JSON_UTIL::firstItem(remainder);
		remainder = JSON_UTIL::removeFirstitem(remainder);
		setSingleConfig(nextKey, value, player);

		nextIndex = remainder.find_first_not_of("\t \n");
		if (nextIndex != std::string::npos && remainder[nextIndex] == ',')
			nextIndex++;
		remainder = remainder.substr(nextIndex);
		nextIndex = remainder.find_first_not_of("\t \n");
	}

}

std::string Race::getConfigString(int player)
{
	std::string configStr = "{ \"menus\" : " + MenuManager::getInstance()->getMenuConfig(); 
	configStr += ", \"achievements\" : " + mAchievements->getCompletedAchievements();
	configStr += "}";

	return configStr;
}

void Race::readConfigStr(int player)
{
	MenuManager *menus = MenuManager::getInstance();

	std::string config = mLogger->getProfileData();
	
	if (config.size() > 0)
	{

		setFromConfigString(config, player);
	}
}


void Race::createStores(Menu *parent, std::vector<Store *> &stores)
{




}

 std::function<void(void)> Race::createFunction(std::string gameType, std::string level)
 {
	 return [this,gameType, level]() { this->mCurrentLevel[gameType] = level; };
 }


bool ReadMapList(std::map<std::string, std::string> &maps, const char *directoryFile)
{
	TiXmlDocument doc(FileUtil::getFullPath(directoryFile).c_str());
	if(!doc.LoadFile(TIXML_ENCODING_UTF8)) {
		return false;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlElement *map;
	TiXmlHandle hRoot(0);

	// <Map>
	map = hDoc.FirstChildElement().Element();
	if (!map)
		return false;
	hRoot = TiXmlHandle(map);

	for (TiXmlElement *mapXML =  hRoot.FirstChildElement("Map").Element(); mapXML; mapXML = mapXML->NextSiblingElement()) 
	{
		const char* fileName = mapXML->GetText();
		const char* prettyName = mapXML->Attribute("Name");
		if (prettyName == NULL)
		{
			prettyName = fileName;
		}
		maps[prettyName] = fileName;
	}
	return true;
}



void
Race::setupMenus(bool loginRequired)
{
    MenuManager *menus = MenuManager::getInstance();

    HUD *h = mHUD;
    MainListener *l = mFrameListener;
    World *w = mWorld;
    Kinect *k = mKinect;
	Achievements *a = mAchievements;
	SoundBank *sb = SoundBank::getInstance();
	Logger *lm = mLogger;
	Player *p = mPlayer;

    Menu *mainMenu = new Menu("Main Menu", "main", 0.05f, 0.1f, 0.08f);
    Menu *options = new Menu("Options", "options", 0.05f, 0.1f, 0.1f, mainMenu);
    Menu *controlOptions = new Menu("Control Options", "controloptions", 0.05f, 0.1f, 0.07f, options);
    Menu *gameplayOptions = new Menu("Gameplay Options", "gameplayoptions", 0.05f, 0.05f, 0.07f, options);
    Menu *advancedOptions = new Menu("Advanced Options", "advancedOptions", 0.05f, 0.1f, 0.08f, options);
    Menu *login = new Menu("Login", "login", 0.05f, 0.1f,0.1f, mainMenu);

	Menu *pauseMenu = new Menu("Pause Menu", "pause", 0.05f, 0.1f);
    Menu *confirmMenu = new Menu("Confirm Profile Reset", "profleReset", 0.1f, 0.1f, 0.1f, advancedOptions);
	Menu *endGameMenu = new Menu("Game Over!", "gameOver", 0.1f, 0.1f, 0.1f, NULL);


	Menu *startGameMenu = new Menu("Choose Race", "startgame", 0.05f, 0.1f, 0.07f, mainMenu);

	Menu *raceMenu = new Menu("Timed Race", "timedRace", 0.05f, 0.1f, 0.08f, startGameMenu); 
	Menu *targetHunt = new Menu("Target Hunt", "targetHunt", 0.05f, 0.1f, 0.08f, startGameMenu); 
	Menu *timedTargetHunt = new Menu("Timed Target Hunt", "timedtargetHunt", 0.05f, 0.1f, 0.08f, startGameMenu); 

//	std::vector<Store *> stores;
//	createStores(mainMenu, stores);
	//for (unsigned int i = 0; i < stores.size(); i++)
	//{
	//	menus->addMenu(stores[i]);

	//}
	//Store *store = stores[0];



	menus->addMenu(mainMenu);
    menus->addMenu(options);
    menus->addMenu(pauseMenu);
	menus->addMenu(gameplayOptions);
	menus->addMenu(controlOptions);
	menus->addMenu(advancedOptions);
	menus->addMenu(login);
	menus->addMenu(endGameMenu);
	menus->addMenu(confirmMenu);
	menus->addMenu(startGameMenu);
	menus->addMenu(raceMenu);
	menus->addMenu(targetHunt);
	menus->addMenu(timedTargetHunt);

	/////////////////////////////////////////////////
	// Login Menu 
	//////////////////////////////////////////////////

	login->AddChooseString("Username",[lm](Ogre::String s) {lm->changeUsername(s); },"",15,false);
	login->AddChooseString("Password",[lm, this](Ogre::String s) {this->setFromConfigString(lm->changePassword(s));},"",15,true);
	login->AddSelectElement("Return to Main Menu", [login, mainMenu]() {login->disable(); mainMenu->enable();});
	
	/////////////////////////////////////////////////
	// Options Menu 
	//////////////////////////////////////////////////

    options->AddSelectElement("Control Options", [options, controlOptions]() {options->disable(); controlOptions->enable();});
    options->AddSelectElement("Gameplay Options", [options, gameplayOptions]() {options->disable(); gameplayOptions->enable();});
    options->AddSelectElement("Advanced Options", [options, advancedOptions]() {options->disable(); advancedOptions->enable();});
	options->AddSelectElement("Return to Main Menu", [options, mainMenu]() {options->disable(); mainMenu->enable();});


	//////////////////////////////////////////////////////
	///  Start Game Menu
	////////////////////////////////////////////////////////

	 startGameMenu->AddSelectElement("Checkpoint Race", [raceMenu,startGameMenu]() {startGameMenu->disable(); raceMenu->enable();});
	 startGameMenu->AddSelectElement("Target Practice", [startGameMenu,targetHunt]() {startGameMenu->disable(); targetHunt->enable();});
	 startGameMenu->AddSelectElement("Timed Target Practice", [startGameMenu,timedTargetHunt]() {startGameMenu->disable(); timedTargetHunt->enable();});
	 startGameMenu->AddSelectElement("Return to Main Menu", [startGameMenu, mainMenu]() {startGameMenu->disable(); mainMenu->enable();});


	//////////////////////////////////////////////////////
	///  Race Menu
	////////////////////////////////////////////////////////


	raceMenu->AddSelectElement("Start", [this, raceMenu]() {raceMenu->disable(), this->startGame("Race", false);});



	std::map<std::string, std::string> levelsRace;
	ReadMapList(levelsRace, "Races.maps");

	std::vector<Ogre::String> levelNamesRace;
	std::vector<std::function<void()>> levelCallbacksRace;



	for (std::map<std::string, std::string>::iterator it = levelsRace.begin();
		 it != levelsRace.end();
		 it++)
	{
		levelNamesRace.push_back((*it).first);
		std::string level = (*it).second;
		levelCallbacksRace.push_back(createFunction("Race", level));
	}

	if (levelsRace.size() > 0)
	{
		mCurrentLevel["Race"] = levelsRace[levelNamesRace[0]];
	}
	raceMenu->AddChooseEnum("Map",levelNamesRace,levelCallbacksRace,0, false);	
	raceMenu->AddChooseInt("Number of Laps", [w](int x) {w->setNumLaps(x); }, 1, 20,w->getNumLaps(), 1, true);
	 raceMenu->AddSelectElement("Return to Game Type Select", [startGameMenu, raceMenu]() {raceMenu->disable(); startGameMenu->enable();});


	//////////////////////////////////////////////////////
	///  Target Menu
	////////////////////////////////////////////////////////

	targetHunt->AddSelectElement("Start", [this, targetHunt]() {targetHunt->disable(), this->startGame("Target", false);});


	std::map<std::string, std::string> levelsTarget;
	ReadMapList(levelsTarget, "Targets.maps");

	std::vector<Ogre::String> levelNameTargets;
	std::vector<std::function<void()>> levelCallbacksTarget;



	for (std::map<std::string, std::string>::iterator it = levelsTarget.begin();
		 it != levelsTarget.end();
		 it++)
	{
		levelNameTargets.push_back((*it).first);
		std::string level = (*it).second;
		levelCallbacksTarget.push_back(createFunction("Target", level));
	}

	if (levelsTarget.size() > 0)
	{
		mCurrentLevel["Target"] = levelsTarget[levelNameTargets[0]];
	}
	targetHunt->AddChooseEnum("Map",levelNameTargets,levelCallbacksTarget,0, false);	

	targetHunt->AddSelectElement("Return to Game Type Select", [startGameMenu, targetHunt]() {targetHunt->disable(); startGameMenu->enable();});



	//////////////////////////////////////////////////////
	///  Target Race Menu
	////////////////////////////////////////////////////////


	timedTargetHunt->AddSelectElement("Start", [this, timedTargetHunt]() {timedTargetHunt->disable(), this->startGame("TargetRace", false);});


	std::map<std::string, std::string> levelsTargetTimed;
	ReadMapList(levelsTargetTimed, "Targets.maps");

	std::vector<Ogre::String> levelNamesTargetTimed;
	std::vector<std::function<void()>> levelCallbacksTargetTimed;



	for (std::map<std::string, std::string>::iterator it = levelsTargetTimed.begin();
		 it != levelsTargetTimed.end();
		 it++)
	{
		levelNamesTargetTimed.push_back((*it).first);
		std::string level = (*it).second;
		levelCallbacksTargetTimed.push_back(createFunction("TargetRace", level));
	}

	if (levelsTargetTimed.size() > 0)
	{
		mCurrentLevel["TargetRace"] = levelsTargetTimed[levelNamesTargetTimed[0]];
	}


	timedTargetHunt->AddChooseEnum("Map",levelNameTargets,levelCallbacksTarget,0, false);	
	timedTargetHunt->AddChooseFloat("Time Limit (minutes)",[w](float x) { w->setTimeLimit(x);}, 0.5, 10, w->getTimeLimit(), 0.25, true);	


	timedTargetHunt->AddSelectElement("Return to Game Type Select", [startGameMenu, timedTargetHunt]() {timedTargetHunt->disable(); startGameMenu->enable();});

	/////////////////////////////////////////////////
	// Options Submenu:  Gameplay 
	//////////////////////////////////////////////////

   
    gameplayOptions->AddChooseFloat("Scale Gates", [w](float x) {w->setScaleGates(x); }, 1,5, w->getScaleGates(), 0.1, true);
    gameplayOptions->AddChooseFloat("Player Speed", [p](float x) {p->setMaxSpeed(x); }, 1.0f, 10.0f, p->getMaxSpeed(), 10, true);
    gameplayOptions->AddChooseFloat("Player Turning Angle (Degrees / sec)", [p](float x) {p->setDegreesPerSecond(x); }, 10, 180, p->getDegressPerSecond(), 0.5f, true);
    gameplayOptions->AddChooseFloat("Player Acceleration", [p](float x) {p->setAcceleration(x); }, 10, 180, p->getAcceleration(), 5, true);

    gameplayOptions->AddChooseBool("Stop When Firing", [p ](bool x) { p->setStopOnFiring(x);},p->getStopOnFiring(), true);
    gameplayOptions->AddChooseBool("NoTurning When Firing", [p ](bool x) { p->setNoTurnOnFiring(x);},p->getNoTurnOnFiring(), true);

    gameplayOptions->AddChooseFloat("Player Laser DPS", [p](float x) {p->setLaserDPS(x); }, 0.5f, 10.0f, p->getLaserDPS(), 0.5f, true);
    gameplayOptions->AddChooseFloat("Laser Duration", [p](float x) {p->setLaserDuration(x); },1.0f, 1000.0f, p->getLaserDuration(), 1.0f, true);
    gameplayOptions->AddChooseFloat("Laser Cooldown", [p](float x) {p->setLaserCooldown(x); },0.0f, 30, p->getLaserCooldown(), 1.0f, true);
	gameplayOptions->AddSelectElement("Return to Options Menu", [gameplayOptions,options]() {gameplayOptions->disable(); options->enable();});



	
	/////////////////////////////////////////////////
	// Options Submenu:  Controls 
	//////////////////////////////////////////////////

//    controlOptions->AddChooseBool("Callibrate Kinect Every Game", [ ](bool x) { },true, true);
	controlOptions->AddChooseFloat("Kinect Sensitivity Left / Right", [k](float x) {k->setSensitivityLR(x);  }, 0.5f, 2.0f,k->getSensitivityLR(), 0.1f, true);
	controlOptions->AddChooseFloat("Kinect Sensitivity Front / Back", [k](float x) {k->SetSensitivityFB(x); }, 0.5f, 2.0f, k->getSensitivityFB(), 0.1f, true);
    controlOptions->AddSelectElement("Callibrate Kinect Now", [controlOptions, k]() {controlOptions->disable(); k->callibrate(4.0f, [controlOptions]() {controlOptions->enable();});});
    controlOptions->AddChooseBool("Invert Front/Back Controls", [](bool x) { },false, true);
	controlOptions->AddChooseBool("Enable Kinect", [p](bool x) { p->setEnableKinect(x);  }, p->getEnableKinect(), true);
	controlOptions->AddChooseBool("Enable Keyboard", [p](bool x) { p->setEnableKeyboard(x);}, p->getEnableKeyboard(), true);
	// controlOptions->AddChooseBool("Enable Gamepad", [p](bool x) { p->setEnableGamepad(x);}, p->getEnableGamepad());


    controlOptions->AddSelectElement("Return to Options Menu", [controlOptions,options]() {controlOptions->disable(); options->enable();});


	/////////////////////////////////////////////////
	// Main Menu 
	//////////////////////////////////////////////////


	mainMenu->AddSelectElement("Select Game", [startGameMenu, mainMenu]() { mainMenu->disable();startGameMenu->enable(); });

	//mainMenu->AddSelectElement("Start Edit", [mainMenu,this]() { mainMenu->disable(); this->startGame("Edit", true); });

	mainMenu->AddSelectElement("Login", [mainMenu, login]() {mainMenu->disable(); login->enable();});
//	mainMenu->AddSelectElement("Show Goals", [mainMenu, a]() {a-> ShowAllAchievements(true); mainMenu->disable();});
	mainMenu->AddSelectElement("Options", [options, mainMenu]() {options->enable(); mainMenu->disable();});
//	mainMenu->AddSelectElement("Store", [store, mainMenu]() {store->enable(); mainMenu->disable();});
	mainMenu->AddSelectElement("Quit", [l, this]() {this->writeConfigStr(); l->quit();});

	/////////////////////////////////////////////////
	// Race Menu 
	//////////////////////////////////////////////////


	/////////////////////////////////////////////////
	// Pause Menu 
	//////////////////////////////////////////////////


    pauseMenu->AddSelectElement("Continue", [pauseMenu,l]() {pauseMenu->disable();l->setPaused(false); });
    pauseMenu->AddSelectElement("End Game (Return to Main Menu)", [pauseMenu,mainMenu, this]() {this->endGame(), pauseMenu->disable();mainMenu->enable(); });
    pauseMenu->AddSelectElement("Quit (Close Program)", [this, l]() {this->writeConfigStr();l->quit();});


	/////////////////////////////////////////////////
	// End Game Menu 
	//////////////////////////////////////////////////

	
    endGameMenu->AddSelectElement("Return to Main Menu", [endGameMenu,mainMenu]() { endGameMenu->disable();mainMenu->enable(); });


	/////////////////////////////////////////////////
	// Options Submenu:  Advanced 
	//////////////////////////////////////////////////


    advancedOptions->AddSelectElement("Get Profile from Server", [this]() {this->readConfigStr();});
    advancedOptions->AddSelectElement("Reset Profile", [advancedOptions, confirmMenu]() {advancedOptions->disable();confirmMenu->enable();});
	advancedOptions->AddChooseFloat("Network Update Freq", [](float x) {Logger::getInstance()->setTimeStep(x); }, 0.1f, 1.f, Logger::getInstance()->getTimeStep(), 0.1f, true);
	advancedOptions->AddSelectElement("Return to Options Menu", [advancedOptions, options]() {advancedOptions->disable(); options->enable();});


    confirmMenu->AddSelectElement("Reset Profile (Cannot be undone!)", [this, advancedOptions, confirmMenu, menus]() {
																											   menus->resetMenus();
																											   this->setupMenus(false);});

    confirmMenu->AddSelectElement("Cancel Profile Reset", [advancedOptions, confirmMenu]() {advancedOptions->enable();confirmMenu->disable();});



	/////////////////////////////////////////////////
	// End of Menu Code
	//////////////////////////////////////////////////

	//mainMenu->enable();
	if (loginRequired)
	{
		login->enable();
	}
	else
	{
		mainMenu->enable();
	}

}

bool 
Race::setup(void)
{

    Ogre::String pluginsPath;
	Ogre::String configPath;
    // only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB

#if OGRE_DEBUG_MODE == 1 && (OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
	// add _d suffix to config files
	pluginsPath = mResourcePath + "plugins_d.cfg";
	configPath = mResourcePath + "ogre_d.cfg";
#else
	pluginsPath = mResourcePath + "plugins.cfg";
	configPath = mResourcePath + "ogre.cfg";

#endif


#endif

    mRoot = new Ogre::Root(pluginsPath, 
        configPath, mResourcePath + "Ogre.log");

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    // Create the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "RaceSMInstance");
    createCamera();
    createViewports();


    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    createScene();
    createFrameListener();
    setupMenus(true);


	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	WSADATA wsa_data;
	if (WSAStartup (MAKEWORD(2,2), &wsa_data) != 0) 
		return true;
    return true;

}
/** Configures the application - returns false if the user chooses to abandon configuration. */
bool 
Race::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true);
        return true;
    }
    else
    {
        return false;
    }
}

void
Race::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;

	#if OGRE_DEBUG_MODE == 1 && (OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
			// add _d suffix to config files
    cf.load(mResourcePath + "resources_d.cfg");
#else
	cf.load(mResourcePath + "resources.cfg");
#endif
		

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                String(macBundlePath() + "/" + archName), typeName, secName);
#else
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
#endif
        }
    }
}

void
Race::destroyScene()
{
    delete mWorld;
    // delete mWorld[1];
    // delete mAIManager;
    delete mRaceCamera;
  //  delete mRaceCamera[1];
	InputHandler::destroyInstance();
}

void 
Race::go(void)
{
    if (!setup())
        return;

    mRoot->startRendering();

    // clean up
    destroyScene();
	SoundBank::shutdown();
}


