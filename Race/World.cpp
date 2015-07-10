#include "World.h"
#include "Race.h"
#include "HUD.h"
#include "OgreSceneManager.h"
#include "Player.h"
#include "GameObject.h"
#include "WaterMesh.h"

#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
#include "InputHandler.h"
#include "OgreQuaternion.h"


#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_NAME "Water/Water2"
#define COMPLEXITY 128 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
#define PLANE_SIZE 8000.0f

int World::worldRand() 
{
    mSeed = mSeed * 1103515245 + 12345;
	int retval = ((unsigned)(mSeed/65536) % RAND_MAX);

    return retval;
}


//float positions[] = {4000,5,4000,
//	                4000,5,3500,
//					3500,5,3500, 
//					3700,5,3700, 
//					3800,5,3700, 
//					3900,5,3700 };

float positions[] = {4000,5,4000,
	                4000,5,5000,
	                5000,5,5000,
					5000,5,4000};


World::World(Ogre::SceneManager *sceneManager, HUD *hud, Race *base) :
	mSceneManager(sceneManager), mBase(base), mHUD(hud)
{
	mSceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));

	Ogre::OverlayManager *om = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *arrowOverlay = om->getByName("HUD/ArrowDirection");

	arrowOverlay->show();

	Ogre::Entity *arrowMesh = mSceneManager->createEntity("Arrow.mesh");
	mArrowNode = mSceneManager->createSceneNode();
	mArrowNode->attachObject(arrowMesh);

	arrowOverlay->add3D(mArrowNode);
	mArrowNode->setPosition(0, 1,-10);
	mArrowNode->pitch(Ogre::Degree(-90));
	//mArrowNode->roll(Ogre::Degree(90));


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

	GameObject *wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(4000,-50,8000), Ogre::Quaternion::IDENTITY);
	wall->setScale(8);
	mStaticObjects.push_back(wall);

	wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(4000,-50,0), Ogre::Quaternion::IDENTITY);
	wall->setScale(8);
	mStaticObjects.push_back(wall);

	wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(0,-50,4000), Ogre::Quaternion::IDENTITY);
	wall->setScale(8);
	wall->yaw(Ogre::Degree(90));
	mStaticObjects.push_back(wall);


	wall = new GameObject(GameObject::STATIC_OBJECT, "TankWall.mesh", mSceneManager,Ogre::Vector3(8000,-50,4000), Ogre::Quaternion::IDENTITY);
	wall->setScale(8);
	wall->yaw(Ogre::Degree(90));
	mStaticObjects.push_back(wall);


	for (int i = 0; i < 100; i++)
	{
		Ogre::Vector3 nextPos(worldRand()  * 8000 / (float) RAND_MAX, 0, worldRand()  * 8000 / (float) RAND_MAX);
		wall = new GameObject(GameObject::STATIC_OBJECT,"Iceberg1.mesh", mSceneManager, nextPos, Ogre::Quaternion::IDENTITY);
		wall->yaw(Ogre::Degree((worldRand() / (float) RAND_MAX) * 360));
		wall->setScale(30);
		mStaticObjects.push_back(wall);
	}



	mGoal = new GameObject(GameObject::GATE, "Goal.mesh",  mSceneManager,Ogre::Vector3(4000,5,4000), Ogre::Quaternion::IDENTITY);
	mGoal->setScale(10);
	mGoal->setAlpha(0.5f);
	mGoal->roll(Ogre::Degree(90));

	mSceneManager->setSkyBox(true, "Skybox/Cloudy");

	for (int i = 0; i < 4; i++)
	{
		mGoalPositions.push_back(Ogre::Vector3(positions[i*3], positions[i*3+1], positions[i*3+2]));
	}

	mCurrentIndex = 0;

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

	if (mPlayer->getSpeed() > 4)
	{
		float displacement = -(Math::Abs(mPlayer->getSpeed()) / (mPlayer->getMaxSpeed() * 10.0f));
		mWaterMesh->push(WaterPlaneX * COMPLEXITY / PLANE_SIZE,WaterPlaneZ * COMPLEXITY / PLANE_SIZE, displacement);
	}

	mWaterMesh->updateMesh(time);

	if (mPlayer->collides(mGoal))
	{
		mCurrentIndex++;
		if (mCurrentIndex >= mGoalPositions.size())
		{
			mCurrentIndex = 0;
		}
		mGoal->setPosition(mGoalPositions[mCurrentIndex]);
	}
     mGoal->pitch(Ogre::Degree(time * 30));

	 Ogre::Vector3 diff = mGoal->getPosition() - mPlayer->getPosition();

	 mArrowNode->getOrientation();

	 Ogre::Quaternion q;
	 q.FromAxes(diff, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Y.crossProduct(diff));

	 mArrowNode->setOrientation(q);

	  InputHandler *ih = InputHandler::getInstance();

	  if (ih->IsKeyDown(OIS::KC_R))
	  {
		  mArrowNode->roll(Ogre::Degree(20*time));
	  }
	  	  if (ih->IsKeyDown(OIS::KC_P))
	  {
		  mArrowNode->pitch(Ogre::Degree(20*time));
	  }
		  	  if (ih->IsKeyDown(OIS::KC_Y))
	  {
		  mArrowNode->yaw(Ogre::Degree(20*time));
	  }


	  


}