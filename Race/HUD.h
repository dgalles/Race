#pragma once
#include <vector>
#include "OgreString.h"
#include "OgreVector3.h"
namespace Ogre
{
	class Overlay;
	class OverlayElement;
}


class HUD
{
public:
	HUD();

	
	void update(float time);

	void showRaceOver(bool show);
	void setSpeed(int speed, bool ghost = false);

	void setTime(float time);
	void setScore(int score);

	void setDebug(float val, std::string msg);
	void setDebug(int val, std::string msg);

	void setDebug(Ogre::Vector3 val, std::string msg);
	void setLaserPower(float laserPercent, bool laserNeedsRecharging);

	void setLap(int currentLap, int numLaps);
	void setGate(int currentGate, int numGates);


	void setTarget(int currentTarget, int numTargets);

	void setRaceDisplay();
	void setTargetDisplay();

	void showHUDElements(bool show);
protected:

	bool mShowHUDElems;
	bool mLaserNeedsRecharging;
		Ogre::Overlay *mScoreOverlay;
	Ogre::OverlayElement *mTimeText;
	Ogre::OverlayElement *mScoreText;
	Ogre::OverlayElement *mSpeedText;
	Ogre::OverlayElement *mLapText;
	Ogre::OverlayElement *mGateText;
	Ogre::OverlayElement *mLaserPercentOverlay;
	Ogre::OverlayElement * mDebug;
	Ogre::OverlayElement *mTargetText;

	Ogre::OverlayElement *mLaser;
};