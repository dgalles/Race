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

	void showHUDElements(bool show);
protected:

	bool mShowHUDElems;
	bool mLaserNeedsRecharging;
		Ogre::Overlay *mScoreOverlay;
	Ogre::OverlayElement *mTimeText;
	Ogre::OverlayElement *mScoreText;
	Ogre::OverlayElement *mSpeedText;
	Ogre::OverlayElement *mLaserPercentOverlay;
	Ogre::OverlayElement * mDebug;

};