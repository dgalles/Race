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

protected:
};