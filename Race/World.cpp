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
#include "Camera.h"
#include "tinyxml.h"
#include "FileUtil.h"
#include "AIManager.h"

#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_NAME "Examples/Water8"
#define COMPLEXITY 32 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
#define PLANE_SIZE 8000

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



bool getSafeString(TiXmlElement *elem, char *name,const char* &c, const char *default = "")
{
	if (elem->FirstChildElement(name))
	{
		c = elem->FirstChildElement(name)->GetText();
	}
	else
	{
		c = default;
	}
	return true;
}

bool getSafeVector3(TiXmlElement *elem, char *name,Ogre::Vector3 &vec)
{
	if (elem->FirstChildElement(name))
	{
		double posX = 0, posY = 0, posZ = 0;
		if (!elem->FirstChildElement(name)->Attribute("X", &posX))
		{
			return false;
		}
		if (!elem->FirstChildElement(name)->Attribute("Y", &posY))
		{
			return false;
		}
		if (!elem->FirstChildElement(name)->Attribute("Z", &posZ))
		{
			return false;
		}
		vec = Ogre::Vector3((float) posX,(float) posY,(float) posZ);
		return true;
	}
	return false;
}
bool getSafeFloat(TiXmlElement *elem, char *name,float &floatVal)
{
	if (elem->FirstChildElement(name))
	{
		floatVal = (float) atof(elem->FirstChildElement(name)->GetText());
		return true;

	}
	return false;
}

void World::LoadMap(std::string mapName)
{
	TiXmlDocument doc(FileUtil::getFullPath(mapName + ".map").c_str());
	if(!doc.LoadFile(TIXML_ENCODING_UTF8)) {
		return;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlElement *map;
	TiXmlHandle hRoot(0);

	// <Map>
	map = hDoc.FirstChildElement().Element();
	if (!map)
		return;
	hRoot = TiXmlHandle(map);







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




	TiXmlElement *Obsticales = hRoot.FirstChildElement("Walls").Element();

	for (TiXmlElement *obs = Obsticales->FirstChildElement(); obs; obs = obs->NextSiblingElement()) 
	{

		// time
		float rotation = 0.0f;
		float scale = 1.0f;
		Ogre::Vector3 nextPos(0.0f);
		getSafeFloat(obs, "Rotation", rotation);
		getSafeFloat(obs, "Scale", scale);
		const char *modelName = "Iceberg1.mesh";
		getSafeString(obs, "Model", modelName,  "Iceberg1.mesh");
		getSafeVector3(obs, "Position", nextPos);

		wall = new GameObject(GameObject::STATIC_OBJECT,modelName, mSceneManager, nextPos, Ogre::Quaternion::IDENTITY);
		wall->yaw(Ogre::Degree(rotation));
		wall->setScale(scale);
		mStaticObjects.push_back(wall);
	}




	TiXmlElement *Enemies = hRoot.FirstChildElement("Enemies").Element();

	if (Enemies != NULL)
	{
		for (TiXmlElement *enemy = Enemies->FirstChildElement(); enemy; enemy = enemy->NextSiblingElement()) 
		{
			float scale = 1.0f;
			Ogre::Vector3 nextPos(0.0f);
			getSafeFloat(enemy, "Scale", scale);
			const char *modelName;
			const char *type = enemy->Value();

			getSafeString(enemy, "Model", modelName, 0);
			getSafeVector3(enemy, "Position", nextPos);

			mAIManager->AddEnemy(type, nextPos,modelName, scale);
		}
	}


	TiXmlElement *gates = hRoot.FirstChildElement("Gates").Element();

	for (TiXmlElement *gate = gates->FirstChildElement(); gate; gate = gate->NextSiblingElement()) 
	{

		// time
		Ogre::Vector3 nextPos(0.0f);
		Ogre::Quaternion orent = Ogre::Quaternion::IDENTITY;
		getSafeVector3(gate, "Position", nextPos);
		mGoalPositions.push_back(nextPos);

	}
	for (unsigned int i = 0; i < mGoalPositions.size(); i++)
	{
		if (i < mGoalPositions.size() - 1)
		{
			Ogre::Vector3 diff = mGoalPositions[i+1] - mGoalPositions[i];
			diff.normalise();
			Ogre::Quaternion orentation;
			orentation.FromAxes( Ogre::Vector3::UNIT_Y.crossProduct(diff), Ogre::Vector3::UNIT_Y,diff);
			mGoalOrientations.push_back(orentation);
		}
		else
		{
			Ogre::Vector3 diff = mGoalPositions[i] - mGoalPositions[i-1];
			diff.normalise();
			Ogre::Quaternion orentation;
			Ogre::Quaternion correct;
			orentation.FromAxes( Ogre::Vector3::UNIT_Y.crossProduct(diff), Ogre::Vector3::UNIT_Y,diff);
			correct.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3::UNIT_X);

			orentation =  orentation * correct;
			mGoalOrientations.push_back(orentation);
		}
	}


	//for (int i = 0; i < 100; i++)
	//{
	//	Ogre::Vector3 nextPos(worldRand()  * 8000 / (float) RAND_MAX, 0, worldRand()  * 8000 / (float) RAND_MAX);
	//	wall = new GameObject(GameObject::STATIC_OBJECT,"Iceberg1.mesh", mSceneManager, nextPos, Ogre::Quaternion::IDENTITY);
	//	wall->yaw(Ogre::Degree((worldRand() / (float) RAND_MAX) * 360));
	//	wall->setScale(30);
	//	mStaticObjects.push_back(wall);
	//}



	for (unsigned int i = 0; i < mNumGoalsToShow && i < mGoalPositions.size(); i++)
	{
		GameObject *goal = new GameObject(GameObject::GATE, "Goal.mesh",  mSceneManager,mGoalPositions[i],  mGoalOrientations[i]);
		goal->setScale(60);
		if (i == 0) 
		{

			goal->setAlpha(1.0f);
		} 
		else
		{
			goal->setMaterial("GoalMat2");
			goal->setAlpha(0.2f);
		}
		//goal->roll(Ogre::Degree(90));
		mGoals.push_back(goal);
	}
	mGoals[0]->setAlpha(1.0f);


	// 	mGoal->setOrientation(mGoal->getOrientation() *);




	mCurrentIndex = 0;


}

World::World(Ogre::SceneManager *sceneManager, HUD *hud, RaceCamera * cam, Race *base) :
	mSceneManager(sceneManager), mBase(base), mHUD(hud), mCamera(cam), mPlayer(NULL)
{
	mRoll = 0;
	mPitch = 0;
	mYaw = 0;
	mSceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));
	mGameRunning = false;
	Ogre::OverlayManager *om = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *arrowOverlay = om->getByName("HUD/ArrowDirection");

	arrowOverlay->show();

	Ogre::Entity *arrowMesh = mSceneManager->createEntity("Arrow.mesh");
	mArrowNode = mSceneManager->createSceneNode();
	mArrowNode->attachObject(arrowMesh);

	arrowOverlay->add3D(mArrowNode);
	mArrowNode->setPosition(0, 3.3f,-10);
	mArrowNode->pitch(Ogre::Degree(-90));
	mArrowNode->roll(Ogre::Degree(90));
	mWorldLoaded = false;

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
	mNumGoalsToShow = 3;





	mSceneManager->setSkyBox(true, "Skybox/Cloudy");


}


void World::AddPlayer(Player *p)
{
	mPlayer = p;
}


void World::StartGame()
{
	if (!mWorldLoaded)
	{
		LoadMap("Level1");
		mWorldLoaded = true;
	}
	mGameRunning = true;
	if (mPlayer != NULL)
	{
		mPlayer->setOrientation(mGoalOrientations[0]);
		mPlayer->setPosition(mGoalPositions[0] - mPlayer->getFacing() * 300);
	}

}

// You'll want various methods to access & change your world here

void World::Think(float time)
{
	if (!mGameRunning)
	{
		return;
	}
	mPlayer->translate(mPlayer->getVelocity() * time);

	float dist = std::numeric_limits<float>::max();
	bool laserCollide = false;
	for (std::vector<GameObject*>::iterator it = mStaticObjects.begin(); it != mStaticObjects.end(); it++)
	{
		Ogre::Vector3 mtd;
		if (mPlayer->collides((*it), mtd))
		{
			mPlayer->translate(mtd);
			mPlayer->setSpeed(0);
		}
		if (mPlayer->isFiringLaser())
		{
			Ogre::Vector3 laserStart;
			Ogre::Vector3 laserDirection;


			mPlayer->getLaser(laserStart, laserDirection);
			if ( (*it)->collides(laserStart, laserDirection, dist))
			{
				laserCollide = true;
			}
		}
	}

	if (laserCollide)
	{
		mPlayer->SetLaserLength(dist);
	}

	float WaterPlaneX = mPlayer->getPosition().x ;
	float WaterPlaneZ = mPlayer->getPosition().z;

	if( Math::Abs(mPlayer->getSpeed()) > 4)
	{
		float displacement = -1 * (Math::Abs(mPlayer->getSpeed()) / (mPlayer->getMaxSpeed() * 5) );
		mWaterMesh->push(WaterPlaneX * COMPLEXITY / PLANE_SIZE,WaterPlaneZ * COMPLEXITY / PLANE_SIZE, displacement);
	}

	mWaterMesh->updateMesh(time);

	if (mPlayer->collides(mGoals[0]))
	{
		mCurrentIndex++;
		if (mCurrentIndex >= (int) mGoalPositions.size())
		{
			mCurrentIndex = 0;
		}
		for (unsigned int i = 0; i < mNumGoalsToShow; i++)
		{
			int index = (mCurrentIndex + i) % mGoalPositions.size();
			mGoals[i]->setPosition(mGoalPositions[index]);
			mGoals[i]->setOrientation(mGoalOrientations[index]);
			//mGoals[i]->roll(Ogre::Degree(90));

		}
	}

	Ogre::Vector3 laserStart;
	Ogre::Vector3 laserDirection;

	if (mPlayer->isFiringLaser())
	{
		mPlayer->getLaser(laserStart, laserDirection);
		mAIManager->rayCollision(laserStart,laserDirection, dist);
	}

	for (unsigned int i = 0; i < mNumGoalsToShow; i++)
	{
		mGoals[i]->roll(Ogre::Degree(time * 30));
	}

	PointArrowAt(mGoals[0]->getPosition());





}


void World::PointArrowAt(Ogre::Vector3 pos)
{
	
	Ogre::Vector3 diff = pos - mPlayer->getPosition();
	diff = mCamera->getOrientation().Inverse() *diff;
	diff.y = 0;

	diff.normalise();

	Ogre::Quaternion q;
	q.FromAxes(Ogre::Vector3::UNIT_Y.crossProduct(diff), Ogre::Vector3::UNIT_Y,diff);

	mArrowNode->setOrientation(q);

	InputHandler *ih = InputHandler::getInstance();

	//if (ih->IsKeyDown(OIS::KC_NUMPAD4))
	//{
	//	mCamera->setOrbitDegree(mCamera->getOrbitDegree() + time*20);

	//}
	//if (ih->IsKeyDown(OIS::KC_NUMPAD6))
	//{
	//	mCamera->setOrbitDegree(mCamera->getOrbitDegree() - time*20);
	//}

	mArrowNode->pitch(Ogre::Degree(90));
	mArrowNode->yaw(Ogre::Degree(90));


}