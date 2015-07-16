#pragma once
#include <vector>
#include "OgreString.h"

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

	void showHUDElements(bool show);
protected:

	bool mShowHUDElems;
		Ogre::Overlay *mScoreOverlay;
	Ogre::OverlayElement *mSpeedText;

};