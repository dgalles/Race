#include "OgreVector3.h"
#include <queue>
#include "Ghost.h"

namespace Ogre {
	class SceneNode;
	class SceneManager;
}

class Player;
class HUD;
class RaceCamera;
class Race;


class World
{
public:


	World(Ogre::SceneManager *sceneManager, HUD *hud, Race *base);

	// You'll want various methods to access & change your world here

	Ogre::SceneManager *SceneManager() { return mSceneManager; }
	HUD *getHUD() { return mHUD; }
	void AddPlayer(Player *p);


	void Think(float time);


protected:

	Ogre::SceneManager *mSceneManager;

	Race *mBase;
	HUD *mHUD;
	Player *mPlayer;

    std::vector<GameObject *> mDynamicObjects;
    std::vector<GameObject *> mStaticObjects;



};
