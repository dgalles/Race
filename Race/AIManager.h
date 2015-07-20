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

	void rayCollision(Ogre::Vector3 rayStart, Ogre::Vector3 rayDirection, float &dist);
	void rayCollision(Ogre::Vector3 rayStart, Ogre::Vector3 rayDirection);

protected:
	World *mWorld;
	Ogre::SceneManager *mSceneManager;
	std::vector<Enemy *> mEnemies;

    // Probablay want a list of individual AI classes here ...
};

#endif