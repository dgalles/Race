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
	mLapText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Laps");
	mGateText= Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Gates");

	mLaser =  Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Laser");
	mLaserPercentOverlay = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Laser/Bar");
	mTargetText= Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Targets");



	mLaserNeedsRecharging = false;

	Ogre::OverlayElement *ghost = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/GhostPanel");
	ghost->hide();

	mDebug =   Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Test");
}



void  HUD::setLap(int currentLap, int numLaps)
{

	mLapText->setCaption("Lap " + std::to_string(currentLap) + " of " + std::to_string(numLaps));

}


void HUD::setTarget(int currentTarget, int numTargets)
{
	mTargetText->setCaption("Targets Destroyed: " + std::to_string(currentTarget) + " of " + std::to_string(numTargets));

}

void HUD::setRaceDisplay()
{
	mTimeText->show();
	mGateText->show();
	mLapText->show();
	mTargetText->hide();
	mLaser->hide();

}
void HUD::setTargetDisplay()
{
	mTimeText->show();
	mGateText->hide();
	mLapText->hide();
	mTargetText->show();
	mLaser->show();

}

void HUD::setDebug(Ogre::Vector3 val, std::string msg)
{
	msg.append(":" + std::to_string(val.x) + "," + std::to_string(val.y) + "," +  std::to_string(val.z)  );

}

void HUD::setGate(int currentGate, int numGates)
{

	mGateText->setCaption("Gate " + std::to_string(currentGate) + " of " + std::to_string(numGates));
}


void HUD::setSpeed(float speed, bool ghost)
{
	char buffer[30];
	sprintf_s(buffer, 30, "Speed = %.1f m / s",speed);
	if (ghost)
	{
		//		mGhostSpeedText->setCaption(("Ghost: " + score));
	}
	else
	{
		mSpeedText->setCaption(buffer);
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



void  HUD::setTime(float val, bool remaining)
{
	char buffer[30];
	if (remaining)
	{
		sprintf_s(buffer, 30, "Time Remaining = %.1f seconds",val);
	}
	else
	{
		sprintf_s(buffer, 30, "Time Elapsed = %.1f seconds",val);
	}

	mTimeText->setCaption(buffer);

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
