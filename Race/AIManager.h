#ifndef __AIManager_h_
#define __AIManager_h_
#include "OgreVector3.h"


class World;
class Enemy;
class Ogre::SceneManager;

class AIManager 
{

public:
    // You may want to add parameters to the AI Manager constructor
	AIManager(World *world, Ogre::SceneManager *sm);
	~AIManager();

    // Go through all of the AIs, and call think on each one
    void Think(float time);

	void AddEnemy(std::string enemyType, Ogre::Vector3 position,const char *model = 0, float scale = 1.0f);

	bool rayCollision(Ogre::Vector3 rayStart, Ogre::Vector3 rayDirection, float &dist, float damage);
	bool rayCollision(Ogre::Vector3 rayStart, Ogre::Vector3 rayDirection, float damage);

	void destroyEnemies();

	const Enemy *getEnemy(int index) { return mEnemies[index]; }
	unsigned int numEnemies() { return mEnemies.size(); }

	const Enemy *getClosestEnemy(Ogre::Vector3 position);


protected:
	World *mWorld;
	Ogre::SceneManager *mSceneManager;
	std::vector<Enemy *> mEnemies;

    // Probablay want a list of individual AI classes here ...
};

#endif