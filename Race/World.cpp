#include "World.h"
#include "Race.h"
#include "HUD.h"
#include "OgreSceneManager.h"
#include "Player.h"
#include "GameObject.h"



World::World(Ogre::SceneManager *sceneManager, HUD *hud, Race *base) :
	mSceneManager(sceneManager), mBase(base), mHUD(hud)
{
		mSceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));

	GameObject *floor = new GameObject(GameObject::STATIC_OBJECT, "TankGroundPlane.mesh", mSceneManager, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);

	GameObject *wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(0,0,500), Ogre::Quaternion::IDENTITY);
	mStaticObjects.push_back(wall);

	wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(0,0,-500), Ogre::Quaternion::IDENTITY);
	mStaticObjects.push_back(wall);


}


void World::AddPlayer(Player *p)
{
	mPlayer = p;
}

// You'll want various methods to access & change your world here

void World::Think(float time)
{

	mPlayer->translate(mPlayer->getVelocity() * time);

		for (std::vector<GameObject*>::iterator it = mStaticObjects.begin(); it != mStaticObjects.end(); it++)
		{
			Ogre::Vector3 mtd;
			if (mPlayer->collides((*it), mtd))
			{
				mPlayer->translate(mtd);
				mPlayer->setSpeed(0);
			}
		}

}