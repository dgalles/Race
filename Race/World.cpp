#include "World.h"
#include "Race.h"
#include "HUD.h"
#include "OgreSceneManager.h"
#include "Player.h"
#include "GameObject.h"
#include "WaterMesh.h"


#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_NAME "Water/Water2"
#define COMPLEXITY 128 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
#define PLANE_SIZE 8000.0f

World::World(Ogre::SceneManager *sceneManager, HUD *hud, Race *base) :
	mSceneManager(sceneManager), mBase(base), mHUD(hud)
{
		mSceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));


		mWaterMesh = new WaterMesh(MESH_NAME, PLANE_SIZE, COMPLEXITY);
		Ogre::Entity *waterEntity = mSceneManager->createEntity(ENTITY_NAME,
                                              MESH_NAME);
		waterEntity->setMaterialName(MATERIAL_NAME);
		SceneNode *waterNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
		waterNode->attachObject(waterEntity);

		mWaterMesh->PARAM_C = 0.3f;
		mWaterMesh->PARAM_D = 0.4f;
		mWaterMesh->PARAM_T = 0.13f;
		mWaterMesh->PARAM_U = 0.05f;

	// GameObject *floor = new GameObject(GameObject::STATIC_OBJECT, "TankGroundPlane.mesh", mSceneManager, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);

	GameObject *wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(4000,0,8000), Ogre::Quaternion::IDENTITY);
	wall->setScale(8);
	mStaticObjects.push_back(wall);

	wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(4000,0,0), Ogre::Quaternion::IDENTITY);
	wall->setScale(8);
	mStaticObjects.push_back(wall);

	wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(0,4000,0), Ogre::Quaternion::IDENTITY);
	wall->setScale(8);
	wall->yaw(Ogre::Degree(90));
	mStaticObjects.push_back(wall);


		wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(8000,4000,0), Ogre::Quaternion::IDENTITY);
	wall->setScale(8);
	wall->yaw(Ogre::Degree(90));
	mStaticObjects.push_back(wall);


	mGoal = new GameObject(GameObject::GATE, "Goal.mesh",  mSceneManager,Ogre::Vector3(0,500,500), Ogre::Quaternion::IDENTITY);
	mGoal->setScale(5);
	mGoal->roll(Ogre::Degree(90));

		mSceneManager->setSkyBox(true, "Skybox/Cloudy");
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
		float WaterPlaneX = mPlayer->getPosition().x ;
		float WaterPlaneZ = mPlayer->getPosition().z;

			mWaterMesh->push(WaterPlaneX * COMPLEXITY / PLANE_SIZE,WaterPlaneZ * COMPLEXITY / PLANE_SIZE, -0.1f);

					mWaterMesh->updateMesh(time);


		mGoal->yaw(Ogre::Degree(time * 20));

}