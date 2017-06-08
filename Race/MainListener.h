#include "Ogre.h"
#include "OgreFrameListener.h"
#include "Kinect_USF.h"

class InputHandler;
class AIManager;
class World;
class RaceCamera;
class Kinect;
class Player;
class HUD;
class MenuManager;
class Achievements;
class Ghost;
class FrameCounter;

namespace Ogre
{
	class RenderWindow;
}

class MainListener : public Ogre::FrameListener
{
public:
 	MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, RaceCamera *cam, Kinect *sensor,  HUD *hud, Achievements *ach, Player *player);

	bool frameStarted(const Ogre::FrameEvent &evt);

	bool paused() { return mPaused; }
	void setPaused(bool paused) { mPaused = paused;}
    void quit() { mQuit = true;}

protected:
	InputHandler *mInputHandler;
	AIManager *mAIManager;
	World *mWorld;
    RaceCamera *mRaceCamera;
	Kinect     *mKinect;
	Achievements *mAchievement;
	HUD *mHUD;
	Ogre::RenderWindow *mRenderWindow;
	bool mPaused;
	MenuManager *mMenus;
    bool mQuit;
	FrameCounter *mFrameCounter;
	Player *mPlayer;
};
