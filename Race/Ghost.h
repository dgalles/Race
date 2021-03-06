#pragma once
#include <vector>
#include "OgreMath.h"

class GameObject;
class World;

class Ghost
{
public:
	Ghost(World *w);
	~Ghost(void);

	void think(float time);
	void record(float time, int segment, float percent, float xdelta, float ydelta, Ogre::Degree lean, Ogre::Degree upDown, 
		        int coins, int distance, int speed);
	void kill(float time);
	void startRecording();
	void stopRecording(bool finishedRace = false);
	void startPlayback();
	void stopPlayback();

	void readFile(std::string filename);
	void writeFile(std::string filename);
	void writeFile();

	void setSampleLength(float newLength) {mSampleLength = newLength; }

	void playerDead(int distance, int coins);


	class GhostInfo
	{
	public:
		float mInitialSpeed;
		float mMaxSpeed;
		bool mUseFrontBack;
		int mAutoAceelerateRate;
		int mManualAceelerateRate;
		int mInitialArmor;
		int mBoostDuration;
		int mShieldDuration;
		int mMagnetDuration;
		int mObsGap;
		float mObjsFreq;
		int mBoostFreq;
		int mShieldFreq;
		int mMagnetFreq;
		int mSeed;
		bool mLeanEqualsDuck;
		bool mRacing;
		int mRaceGoal;
		int mRaceType;
	};

	GhostInfo *getData() { return mGhostInfo; }
protected:

	class GhostData
	{
	public:
		GhostData() {}

		GhostData(float time, int segment, float percent, float xdelta, float ydelta, Ogre::Degree lean, Ogre::Degree upDown, int coins, int distance, int speed):
			mTime(time), mSegment(segment), mPercent(percent), mXdelta(xdelta), mYDelta(ydelta), mLean(lean), mUpDown(upDown), mCoins(coins), mDistance(distance), mSpeed(speed) {}

		float mTime;
		int mSegment;
		float mPercent;
		float mXdelta;
		float mYDelta;
		Ogre::Degree mUpDown;
		Ogre::Degree mLean;
		int mCoins;
		int mDistance;
		int mSpeed;


	};

	void updateGhost(float time);

	float mFinalTime;
	bool mFinishedRace;
	bool mPlayingBack;
	bool mRecording;
	float mCurrentTime;
	float mLastRecordTime;
	int mSeed;
	GameObject *mGameObject;
	float mKillTime;
	float mSampleLength;
	bool mAlive;
	int mDataindex;

	Ghost::GhostInfo *mGhostInfo;
	char mTimeBuf[sizeof "2011_10_08_07_07_09"];

	std::vector<GhostData> mData;
	World *mWorld;

};

