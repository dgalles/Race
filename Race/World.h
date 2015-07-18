#include "OgreVector3.h"
#include "OgreQuaternion.h"
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
class WaterMesh;
class AIManager;
class World
{
public:


	World(Ogre::SceneManager *sceneManager, HUD *hud, RaceCamera * cam, Race *base);

	void AddAIManager(AIManager *am) {mAIManager = am; }


	Ogre::SceneManager *SceneManager() { return mSceneManager; }
	HUD *getHUD() { return mHUD; }
	void AddPlayer(Player *p);


	void Think(float time);

	int worldRand();
	void StartGame();

	RaceCamera *getCamera() { return mCamera; }
	void LoadMap(std::string map);

protected:


	Ogre::SceneManager *mSceneManager;

	Race *mBase;
	HUD *mHUD;
	Player *mPlayer;

    std::vector<GameObject *> mDynamicObjects;
    std::vector<GameObject *> mStaticObjects;
	std::vector<GameObject *> mGoals;
	WaterMesh *mWaterMesh;
	int mSeed;
	int goalIndex;
	std::vector<Ogre::Vector3> mGoalPositions;
	std::vector<Ogre::Quaternion> mGoalOrientations;
	int mCurrentIndex ;
	Ogre::SceneNode *mArrowNode;
	RaceCamera* mCamera;
	float mRoll;
	float mPitch;
	float mYaw;
	unsigned int mNumGoalsToShow;
	AIManager *mAIManager;
	bool mGameRunning;
	bool mWorldLoaded;
};
