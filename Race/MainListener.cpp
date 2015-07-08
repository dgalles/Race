#include "MainListener.h"
#include "AIManager.h"
#include "InputHandler.h"
#include "World.h"
#include "Camera.h"
#include "Kinect.h"
#include "Player.h"
#include "HUD.h"
#include "Menu.h"
#include "Achievements.h"
#include "Ghost.h"
#include "FrameCounter.h"
#include <stdio.h>


MainListener::MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, RaceCamera *cam, Kinect *sensor,  HUD *hud, Achievements *ach, Player *player) :
mRenderWindow(window), mAIManager(aiManager), mHUD(hud),  mKinect(sensor), mWorld(world), mPlayer(player)
{
		mRaceCamera = cam;
		mAchievement = ach;
	mPaused = false;
	mQuit = false;
	mFrameCounter = new FrameCounter();
	//mInputHandler->setFrameListener(this);
}


// On every frame, call the appropriate managers
bool 
	MainListener::frameStarted(const Ogre::FrameEvent &evt)
{
	
	float time = evt.timeSinceLastFrame;
	if (time > 0.5)
	{
		time = 0.5;
	}

	
 	mKinect->update(evt.timeSinceLastFrame);
	mHUD->update(time);
	InputHandler::getInstance()->Think(time);
	mFrameCounter->update(time);
	if (!mPaused)
	{
		mAIManager->Think(time);
		//  The only reason we have the Think method of the InputHandler return
		//   a value, is for the escape key to cause our application to end.
		//   Feel free to change this to something that makes more sense to you.
		for (int i = 0; i < 1; i++)
		{
			mWorld->Think(time);
			mPlayer->Think(time);
			mRaceCamera->Think(time);
			mAchievement->Think(time);
		}
	}
	MenuManager::getInstance()->think(time);

    bool keepGoing = true;

    if (InputHandler::getInstance()->IsKeyDown(OIS::KC_ESCAPE))
    {
        if (mKinect->callibrating())
        {
            mKinect->cancelCallibration();
        }
        else if (MenuManager::getInstance()->getActiveMenu() != NULL)
        {
            // do nothing
        }
        else if (mAchievement->ShowingAllAchievements())
		{
			mAchievement->ShowAllAchievements(false);
            MenuManager::getInstance()->getMenu("main")->enable();

		}
		else
        {
			mAchievement->clearUI();
            MenuManager::getInstance()->getMenu("pause")->enable();
        }
    }

	// Ogre will shut down if a listener returns false.  We will shut everything down if
	// either the user presses escape or the main render window is closed.  Feel free to 
	// modify this as you like.
	if ( mRenderWindow->isClosed() || mQuit)
	{
		keepGoing = false;
	//	mKinect->shutdown();
	}
	return keepGoing;
}