#include "HUD.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
#include <OgreTextAreaOverlayElement.h>
#include "OgreFontManager.h"
#include "OgreOverlayElement.h"

HUD::HUD()
{
	mShowHUDElems = true;
	mScoreOverlay = Ogre::OverlayManager::getSingleton().getByName("HUD/Score");

	mSpeedText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Speed");

}

void HUD::setSpeed(int speed, bool ghost)
{
	std::string score = "Speed = ";
	score.append(std::to_string((long long)speed));
	score.append(" m / s");
	if (ghost)
	{
		//		mGhostSpeedText->setCaption(("Ghost: " + score));
	}
	else
	{
		mSpeedText->setCaption(score);
	}
}
void HUD::update(float time)
{


}

void HUD::showRaceOver(bool show)
{


}
void HUD::showHUDElements(bool show)
{
	mShowHUDElems = show;
	if (show)
	{
		mScoreOverlay->show();
		//if(mShowGhostElements)
		//{
		//	Ogre::OverlayElement *ghost = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/GhostPanel");
		//	ghost->show();
		//}
	}
	else
	{
		mScoreOverlay->hide();
	}
}
