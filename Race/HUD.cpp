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
	mScoreOverlay->show();
	mSpeedText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Speed");
	mScoreText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Score");
	mTimeText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Time");
	mLaserPercentOverlay = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Laser/Bar");
	mLaserNeedsRecharging = false;

	mDebug =   Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Test");
}


void HUD::setDebug(Ogre::Vector3 val, std::string msg)
{
		msg.append(":" + std::to_string(val.x) + "," + std::to_string(val.y) + "," +  std::to_string(val.z)  );

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


void  HUD::setDebug(float val, std::string msg)
{
	msg.append(":" + std::to_string( val));
	mDebug->setCaption(msg);
}

void  HUD::setDebug(int val, std::string msg)
{
	msg.append(":" + std::to_string(val));
	mDebug->setCaption(msg);

}



void  HUD::setTime(float val)
{
	std::string time = "Time = ";
	time.append(std::to_string(val));
	mTimeText->setCaption(time);

}
void  HUD::setScore(int val)
{
	std::string score = "Score = ";
	score.append(std::to_string(val));
	mScoreText->setCaption(score);
}

void HUD::update(float time)
{


}

void HUD::setLaserPower(float time, bool needsRecarge)
{
	if (mLaserNeedsRecharging != needsRecarge)
	{
		mLaserNeedsRecharging = needsRecarge;
		if (mLaserNeedsRecharging)
		{
			mLaserPercentOverlay->setMaterialName("LaserPowerRecharge");
		}
		else
		{
			mLaserPercentOverlay->setMaterialName("LaserPowerOK");
		}

	}
	mLaserPercentOverlay->setWidth(time * 0.5f);
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
