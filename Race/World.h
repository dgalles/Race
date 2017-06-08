#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include <queue>
#include "Ghost.h"
#include "Receivers.h"

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
	void StartGame(const char *worldName, bool timeLimit = false, bool doEdit = false);

	RaceCamera *getCamera() { return mCamera; }
	void LoadMap(std::string map);
	void destroyWorld();
	void enemyDestroyed(float value);
	int getNumLaps() { return mNumLaps; }
	void setNumLaps(int laps) { mNumLaps = laps; }
	void addPlayerListener(PlyrDataMsgr *listener) { mLoggersToSend.push_back(listener); }

	float getTimeLimit() { return mTimeLimit; }
	void setTimeLimit(float newLimit) { mTimeLimit = newLimit; }

	enum GameType {RACE, TARGET};

	float getScaleGates() { return mScaleGates;}
	void setScaleGates(float newScale) {mScaleGates = newScale; }

	GameType getGameType() {return mGameType;}

protected:
	void doEdit(float time);
	void SaveFile(char *filename);
	void unselectAll();
	void reloadForEdit();
	float mTimeLimit;
	float mCurrentTimeLimit;
	bool mTimed;
	GameType mGameType;
	void finishRace();


	float mScaleGates;

	void PointArrowAt(Ogre::Vector3 pos);

	Ogre::SceneManager *mSceneManager;

	Race *mBase;
	HUD *mHUD;
	Player *mPlayer;

	int mNumLaps;

	float mDisp;
	float mTimeSinceLastLog;

    std::vector<GameObject *> mDynamicObjects;
    std::vector<GameObject *> mStaticObjects;
	std::vector<GameObject *> mGoals;
	WaterMesh *mWaterMesh;
	int mSeed;
	int goalIndex;
	std::vector<Ogre::Vector3> mGoalPositions;
	std::vector<float> mGoalScales;
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
	float mCurrentTime;
	bool mEditing;


	int mGatesHit;
	float mTimeHittingTarget;
	float mTimeMissingTarget;

	
	GameObject *mCurrentEditObject;

	int mCurrentLap;

	int mCurrentGateIndex;
	int mCurrentStaticIndex;
	int mCurrentDynamicIndex;

	enum ObjectSelect {GATE, STATIC, DYNAMIC};
	ObjectSelect mSelectedType;

	int mInitialTargets;
	float mPlacementScale;

		std::vector<PlyrDataMsgr *> mLoggersToSend;



};
