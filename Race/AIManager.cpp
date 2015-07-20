#include "AIManager.h"
#include "World.h"
#include "Enemy.h"
#include "OgreSceneManager.h"

AIManager::AIManager(World *world, Ogre::SceneManager *sm) : mWorld(world), mSceneManager(sm)
{
    // Any initialization that needs to be done ...
}

AIManager::~AIManager()
{
    // Clean up after yourself ...
}

void 
AIManager::Think(float time)
{
			for (std::vector<Enemy *>::iterator it = mEnemies.begin(); it != mEnemies.end(); it++)
			{

				(*it)->Think(time);
			}
}

void AIManager::rayCollision(Ogre::Vector3 rayStart, Ogre::Vector3 rayDirection, float &dist)
{
	for (std::vector<Enemy *>::iterator it = mEnemies.begin(); it != mEnemies.end(); it++)
	{
		if ((*it)->collides(rayStart, rayDirection, dist))
		{
			(*it)->setMaterial("simpleRed");

		}
		else
		{
			(*it)->restoreOriginalMaterial();

		}
	}

}

void AIManager::rayCollision(Ogre::Vector3 rayStart, Ogre::Vector3 rayDirection)
{
	for (std::vector<Enemy *>::iterator it = mEnemies.begin(); it != mEnemies.end(); it++)
	{
		float dummy =  std::numeric_limits<float>::max();
		if ((*it)->collides(rayStart, rayDirection, dummy))
		{
			(*it)->setMaterial("simpleRed");

		}
		else
		{
			(*it)->restoreOriginalMaterial();

		}
	}

}



void AIManager::AddEnemy(std::string enemyType, Ogre::Vector3 position, const char *model /* = 0 */, float scale /* = 1.0f */ )
{
	if (enemyType == std::string("Target"))
	{
		Target *t = new Target();
		if (model == 0)
		{
			model = "Target.mesh";
			scale = 10.0f;
		}
		t->loadModel(model, mSceneManager);
		t->setScale(scale);
		t->setPosition(position);
		mEnemies.push_back(t);
	}

}
