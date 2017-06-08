#include "AIManager.h"
#include "World.h"
#include "Enemy.h"
#include "OgreSceneManager.h"
#include "HUD.h"
#include "Sound.h"

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


// TODO:  Make this more efficient!
const Enemy *AIManager::getClosestEnemy(Ogre::Vector3 position)
{
	Enemy *closestEnemy = NULL;
	float closestSquaredDist = 0.0f;
	for (std::vector<Enemy *>::iterator it = mEnemies.begin(); it != mEnemies.end(); it++)
	{
		if (closestEnemy == NULL)
		{
			closestEnemy = (*it);
			closestSquaredDist = (*it)->getPosition().squaredDistance(position);
		}
		else
		{
			float squareDist = (*it)->getPosition().squaredDistance(position);
			if (squareDist < closestSquaredDist)
			{
				closestSquaredDist = squareDist;
				closestEnemy = (*it);
			}
		}

	}
	return closestEnemy;
}

bool AIManager::rayCollision(Ogre::Vector3 rayStart, Ogre::Vector3 rayDirection, float &dist, float damage)
{
	std::vector<Enemy *>dead;

	bool hitany = false;

	for (std::vector<Enemy *>::iterator it = mEnemies.begin(); it != mEnemies.end(); it++)
	{
		if ((*it)->collides(rayStart, rayDirection, dist))
		{
			hitany = true;
			(*it)->decreaseHealth(damage);

		//	mWorld->getHUD()->setDebug((*it)->getHealth(), "Health");
			if ((*it)->isDead())
			{
				SoundBank::getInstance()->play("explode1");
				dead.push_back(*it);
				mWorld->enemyDestroyed((*it)->getValue());
			}

		}
	}
	for (std::vector<Enemy *>::iterator it = dead.begin(); it != dead.end(); it++)
	{
		mEnemies.erase(std::remove(mEnemies.begin(), mEnemies.end(), (*it)),mEnemies.end());
		delete			(*it);
	}
	return hitany;
}

void AIManager::destroyEnemies()
{
	for (std::vector<Enemy *>::iterator it = mEnemies.begin(); it != mEnemies.end(); it++)
	{
		delete (*it);
	}
	mEnemies.clear();
}



bool AIManager::rayCollision(Ogre::Vector3 rayStart, Ogre::Vector3 rayDirection, float damage)
{
	float dummy =  std::numeric_limits<float>::max();
	return rayCollision(rayStart, rayDirection, dummy, damage);
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
