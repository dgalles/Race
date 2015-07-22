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

#include <iostream>
#include <fstream>

// HACK!
#include "Enemy.h"

#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_NAME "Examples/Water8"
#define COMPLEXITY 128 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
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


bool getSafeBool(TiXmlElement *elem, char *name,bool &boolVal)
{
	if (elem->FirstChildElement(name))
	{
		std::string text(elem->FirstChildElement(name)->GetText());

		std::transform(text.begin(), text.end(), text.begin(), ::toupper);
		boolVal = text.compare(std::string("TRUE")) == 0 || text.compare(std::string("1")) == 0;
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



	TiXmlElement *config = hRoot.FirstChildElement("Config").Element();
	if (config)
	{
		 bool restrictElev = false;
		 getSafeBool(config, "RestrictGunElevation", restrictElev);
		 mPlayer->setLaserAimVertical(!restrictElev);

		 bool useGun = true;
		 getSafeBool(config, "AllowGun", useGun);
		 mPlayer->setLaserAllowed(useGun);

	}



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

	if (Obsticales != NULL)
	{
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

	if (gates != NULL)
	{
	for (TiXmlElement *gate = gates->FirstChildElement(); gate; gate = gate->NextSiblingElement()) 
	{

		// time
		Ogre::Vector3 nextPos(0.0f);
		Ogre::Quaternion orent = Ogre::Quaternion::IDENTITY;
		getSafeVector3(gate, "Position", nextPos);
		mGoalPositions.push_back(nextPos);

	}
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
			orentation.FromAxes( Ogre::Vector3::UNIT_Y.crossProduct(diff), Ogre::Vector3::UNIT_Y,diff);
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
	
	if (mGoals.size() >  0)
	{
		mGoals[0]->setAlpha(1.0f);
	}

	// 	mGoal->setOrientation(mGoal->getOrientation() *);

	mDisp = -1.0f;


	mCurrentIndex = 0;


}


	void World::enemyDestroyed(float value)
	{


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


	mPlacementScale = 1.0f;
	mCurrentEditObject = 0;

	mSceneManager->setSkyBox(true, "Skybox/Cloudy");


}


void World::AddPlayer(Player *p)
{
	mPlayer = p;
}


void World::destroyWorld()
{
		for (std::vector<GameObject*>::iterator it = mStaticObjects.begin(); it != mStaticObjects.end(); it++)
		{
			delete (*it);
		}
		for (std::vector<GameObject*>::iterator it = mGoals.begin(); it != mGoals.end(); it++)
		{
			delete (*it);
		}
		mStaticObjects.clear();
		mGoals.clear();
		mAIManager->destroyEnemies();
		mWorldLoaded = false;

}


void World::reloadForEdit()
{
		for (std::vector<GameObject*>::iterator it = mGoals.begin(); it != mGoals.end(); it++)
		{
			delete (*it);
		}
		mGoals.clear();

		for (unsigned int i = 0; i < mGoalPositions.size(); i++)
		{

			GameObject *goal = new GameObject(GameObject::GATE, "Goal.mesh",  mSceneManager,mGoalPositions[i],  mGoalOrientations[i]);
			goal->setScale(60);

			mGoals.push_back(goal);
		}
		
		if (mGoals.size() > 0)
		{
			mCurrentGateIndex = mGoals.size() -1 ;
			mGoals[mCurrentGateIndex]->setMaterial("simpleRed");
		}

		mCurrentStaticIndex = mStaticObjects.size() - 1;

		for (unsigned int i = 0; i < mAIManager->numEnemies(); i++)
		{
			const Enemy *obj = mAIManager->getEnemy(i);

			GameObject *target = (new GameObject(GameObject::GATE, "Target.mesh",  mSceneManager,obj->getPosition(), mPlayer->getOrientation()));
			target->setScale(20);

			mDynamicObjects.push_back(target);
		}
		mAIManager->destroyEnemies();
		mCurrentDynamicIndex = mDynamicObjects.size() - 1;

}

void World::StartGame(char *worldname, bool doEdit)
{
	if (!mWorldLoaded)
	{
		LoadMap(worldname);
		mWorldLoaded = true;
	}
	mGameRunning = true;
	if (doEdit)
	{
		mEditing = true;
		mPlayer->setLaserAllowed(false);
		mPlayer->setOrientation(Ogre::Quaternion::IDENTITY);
		mPlayer->setPosition( Ogre::Vector3(4000, 0, 100));
		reloadForEdit();
	}
	else
	{
		mEditing = false;
		if (mPlayer != NULL)
		{
			mPlayer->setOrientation(mGoalOrientations[0]);
			mPlayer->setPosition(mGoalPositions[0] - mPlayer->getFacing() * 300);
		}
		mCurrentTime = 0;


	}

}

// You'll want various methods to access & change your world here

void World::SaveFile(char *filename)
{
	std::ofstream outFile;
	outFile.open (filename, std::ios::out);

	outFile << "<Map> \n";

	outFile << "<Enemies> \n";

		for (std::vector<GameObject *>::iterator it = mDynamicObjects.begin(); it != mDynamicObjects.end(); it++)
	{
		Ogre::Vector3 pos = (*it)->getPosition();
		outFile << "  <Target>\n";
		outFile << "    <Type>Target</Type> \n";
		outFile << "    <Scale>20</Scale>\n";
		outFile << "    <Model>Target.mesh</Model>\n";
		outFile << "     <Position X=\"" << (int)(pos.x) << "\" Y=\"" << (int)(pos.y) << "\" Z=\"" <<  (int)(pos.z) << "\" />\n";
		outFile << "  </Target>\n";	
	}
	outFile << "</Enemies> \n";

		outFile << "<Gates> \n";
		for (std::vector<GameObject *>::iterator it = mGoals.begin(); it != mGoals.end(); it++)
	{
		Ogre::Vector3 pos = (*it)->getPosition();
		outFile << "  <Gate>\n";
		outFile << "     <Position X=\"" << (int)(pos.x) << "\" Y=\"" << (int)(pos.y) << "\" Z=\"" <<  (int)(pos.z) << "\" />\n";
		outFile << "  </Gate>\n";	
	}
	outFile << "</Gates> \n";

			outFile << "<Walls> \n";
		for (unsigned int i = 4; i < mStaticObjects.size(); i++)
	{
		GameObject *ob = mStaticObjects[i];
		Ogre::Vector3 pos =ob->getPosition();
		Ogre::Quaternion q = ob->getOrientation();

		outFile << "  <Wall>\n";
		outFile << "     <Position X=\"" << (int)(pos.x) << "\" Y=\"" << (int)(pos.y) << "\" Z=\"" <<  (int)(pos.z) << "\" />\n";
		outFile << "     <Scale>" << ob->getScale().x << "</Scale>\n";
		outFile << "     <Rotation>" << q.getYaw().valueDegrees() << "</Rotation>\n";
		outFile << "     <Model>Iceberg1.mesh</Model> \n";
		outFile << "  </Wall>\n";	
	}
	outFile << "</Walls> \n";


	outFile << "</Map> \n";
	outFile.close();
}



void World::unselectAll()
{
	if (mCurrentGateIndex >= 0 && mCurrentGateIndex < (int) mGoals.size())
	{
		mGoals[mCurrentGateIndex]->restoreOriginalMaterial();
	}
	if (mCurrentStaticIndex >= 4 && mCurrentStaticIndex < (int) mStaticObjects.size())
	{
		mStaticObjects[mCurrentStaticIndex]->restoreOriginalMaterial();
	}
	if (mCurrentDynamicIndex >= 0 && mCurrentDynamicIndex <(int)  mDynamicObjects.size())
	{
		mDynamicObjects[mCurrentDynamicIndex]->restoreOriginalMaterial();
	}

}

void World::doEdit(float time)
{

	InputHandler *ih = InputHandler::getInstance();

	Ogre::Vector3 facing = mPlayer->getFacing();

	Ogre::Vector3 left = Ogre::Vector3::UNIT_Y.crossProduct(facing);


	if (!ih->IsKeyDown(OIS::KC_2))
	{
		mCurrentEditObject = NULL;
	}

	float mult = 200 * time;
	if (ih->IsKeyDown(OIS::KC_LSHIFT))
	{
		mult *= 5;
	}
	if (ih->IsKeyDown(OIS::KC_W))
	{
		mPlayer->translate(facing * mult);
	}
	if (ih->IsKeyDown(OIS::KC_S))
	{
		mPlayer->translate(-facing *mult);
	}
	if (ih->IsKeyDown(OIS::KC_A))
	{
		mPlayer->translate(left * mult);
	}
	if (ih->IsKeyDown(OIS::KC_D))
	{
		mPlayer->translate(-left * mult);
	}

	if (ih->IsKeyDown(OIS::KC_LEFT) &&ih->IsKeyDown(OIS::KC_LSHIFT) )

	{
		mPlayer->yaw(Ogre::Degree(45 * time), false);
	}
	if (ih->IsKeyDown(OIS::KC_RIGHT) &&ih->IsKeyDown(OIS::KC_LSHIFT) )

	{
		mPlayer->yaw(Ogre::Degree(-45 * time), false);
	}

	// TODO:  Make this work with non-statics
	if (mCurrentEditObject != NULL)
	{
		mCurrentEditObject->setScale(mPlacementScale);
		Ogre::Vector3 pos = mPlayer->getPosition();
		mCurrentEditObject->setPosition(mPlayer->getPosition());
		Ogre::Quaternion orient(left, Ogre::Vector3::UNIT_Y, facing);
		mCurrentEditObject->setOrientation(orient);
	}



//	mPlayer->translate(mPlayer->getVelocity() * time);
//	mHUD->setDebug(mPlayer->getPosition(), "Pos");

	if (ih->KeyPressedThisFrame(OIS::KC_P))
	{
		SaveFile("TestLevel.level");
	}


	if (ih->IsKeyDown(OIS::KC_SPACE))
	{
		mPlayer->setSpeed(0);
	}


	if (ih->KeyPressedThisFrame(OIS::KC_1))
	{
		// If a gate isn't selected, insert at the end of the gate list
		if (mSelectedType != ObjectSelect::GATE)
		{
			mCurrentGateIndex = mGoals.size() - 1;
		}
		mHUD->setDebug(mPlayer->getPosition(), "Gate Placed");
		GameObject *goal = new GameObject(GameObject::GATE, "Goal.mesh",  mSceneManager,mPlayer->getPosition(), Ogre::Quaternion::IDENTITY);
		goal->setScale(60);
		goal->setMaterial("simpleRed");
		mSelectedType = ObjectSelect::GATE;

		if (mGoals.size() == 0)
		{
			mGoals.push_back(goal);
			mCurrentGateIndex = 0;
		}
		else
		{
			mGoals[mCurrentGateIndex]->restoreOriginalMaterial();
			mCurrentGateIndex++;

			mGoals.insert(mGoals.begin()+ mCurrentGateIndex, goal);
			if (mCurrentGateIndex > 0)
			{
				Ogre::Vector3 diff = mGoals[mCurrentGateIndex]->getPosition() - mGoals[mCurrentGateIndex-1]->getPosition();
				diff.normalise();
				Ogre::Quaternion orentation;
				orentation.FromAxes( Ogre::Vector3::UNIT_Y.crossProduct(diff), Ogre::Vector3::UNIT_Y,diff);
				mGoals[mCurrentGateIndex-1]->setOrientation(orentation);
			}
			if (mGoals.size() > 1)
			{
				int nextGoalIndex = (mCurrentGateIndex + 1) % mGoals.size();
				Ogre::Vector3 diff = mGoals[nextGoalIndex]->getPosition() - mGoals[mCurrentGateIndex]->getPosition();
				diff.normalise();
				Ogre::Quaternion orentation;
				orentation.FromAxes( Ogre::Vector3::UNIT_Y.crossProduct(diff), Ogre::Vector3::UNIT_Y,diff);
				mGoals[mCurrentGateIndex]->setOrientation(orentation);

			}



		}
	}
	if(ih->KeyPressedThisFrame(OIS::KC_DELETE) && mSelectedType == ObjectSelect::GATE)
	{
		if ((mGoals.size() > 0) && mCurrentGateIndex >= 0 && mCurrentGateIndex < (int) mGoals.size())
		{
			GameObject *ob = mGoals[mCurrentGateIndex];
			mGoals.erase(mGoals.begin() + mCurrentGateIndex);
			mCurrentGateIndex--;
			if (mCurrentGateIndex < 0)
			{
				mCurrentGateIndex = 0;
			}
			if (mGoals.size() > 0)
			{
				mGoals[mCurrentGateIndex]->setMaterial("simpleRed");
			}
			if (mCurrentGateIndex < (int) mGoals.size() - 1)
			{
				Ogre::Vector3 diff = mGoals[mCurrentGateIndex+1]->getPosition() - mGoals[mCurrentGateIndex]->getPosition();
				diff.normalise();
				Ogre::Quaternion orentation;
				orentation.FromAxes( Ogre::Vector3::UNIT_Y.crossProduct(diff), Ogre::Vector3::UNIT_Y,diff);
				mGoals[mCurrentGateIndex]->setOrientation(orentation);
			}

			delete ob;
		}
	}

	if(ih->KeyPressedThisFrame(OIS::KC_DELETE) && mSelectedType == ObjectSelect::STATIC)
	{
		if ((mStaticObjects.size() > 4) && mCurrentStaticIndex >= 4 && mCurrentStaticIndex < (int) mStaticObjects.size())
		{
			GameObject *ob = mStaticObjects[mCurrentStaticIndex];
			mStaticObjects.erase(mStaticObjects.begin() + mCurrentStaticIndex);
			mCurrentStaticIndex--;
			if (mCurrentStaticIndex < 4)
			{
				mCurrentStaticIndex = 4;
			}
			if ((int) mStaticObjects.size() > mCurrentStaticIndex)
			{
				mStaticObjects[mCurrentStaticIndex]->setMaterial("simpleRed");
			}
			delete ob;
		}
	}

		if(ih->KeyPressedThisFrame(OIS::KC_DELETE) && mSelectedType == ObjectSelect::DYNAMIC)
	{
		if (((int) mDynamicObjects.size() > 0) && mCurrentDynamicIndex >= 0 && mCurrentDynamicIndex < mDynamicObjects.size())
		{
			GameObject *ob = mDynamicObjects[mCurrentDynamicIndex];
			mDynamicObjects.erase(mDynamicObjects.begin() + mCurrentDynamicIndex);
			mCurrentDynamicIndex--;
			if (mCurrentDynamicIndex < 0)
			{
				mCurrentDynamicIndex = 0;
			}
			if ((int) mDynamicObjects.size() > mCurrentDynamicIndex)
			{
				mDynamicObjects[mCurrentDynamicIndex]->setMaterial("simpleRed");
			}
			delete ob;
		}
	}

	if (ih->KeyPressedThisFrame(OIS::KC_LBRACKET))
	{
		unselectAll();
		if (!ih->IsKeyDown(OIS::KC_LSHIFT) && !ih->IsKeyDown(OIS::KC_LCONTROL))
		{
			if (mCurrentGateIndex > 0)
			{
				mCurrentGateIndex--;
			}
			if (mGoals.size() > 0)
			{
				mGoals[mCurrentGateIndex]->setMaterial("simpleRed");
				mSelectedType = ObjectSelect::GATE;
			}

		} 
		else if(ih->IsKeyDown(OIS::KC_LSHIFT))
		{
			if (mCurrentStaticIndex > 4)
			{
				mCurrentStaticIndex--;
			}
			if (mStaticObjects.size() > 4)
			{
				mStaticObjects[mCurrentStaticIndex]->setMaterial("simpleRed");
				mSelectedType = ObjectSelect::STATIC;
			}
		} 
		else  if(ih->IsKeyDown(OIS::KC_LCONTROL))
		{
			if (mCurrentDynamicIndex > 0)
			{
				mCurrentDynamicIndex--;
			}
			if (mDynamicObjects.size() > 0)
			{
				mDynamicObjects[mCurrentDynamicIndex]->setMaterial("simpleRed");
				mSelectedType = ObjectSelect::DYNAMIC;
			}
		}
	}
	if (ih->KeyPressedThisFrame(OIS::KC_RBRACKET))
			{
		unselectAll();
		if (!ih->IsKeyDown(OIS::KC_LSHIFT) && !ih->IsKeyDown(OIS::KC_LCONTROL))
		{
			if (mCurrentGateIndex < (int) mGoals.size() - 1)
			{
				mCurrentGateIndex++;
			}
			if (mGoals.size() > 0)
			{
				mGoals[mCurrentGateIndex]->setMaterial("simpleRed");
				mSelectedType = ObjectSelect::GATE;
			}

		} 
		else if(ih->IsKeyDown(OIS::KC_LSHIFT))
		{
			if (mCurrentStaticIndex < mStaticObjects.size() - 1)
			{
				mCurrentStaticIndex++;
			}
			if (mStaticObjects.size() > 4)
			{
				mStaticObjects[mCurrentStaticIndex]->setMaterial("simpleRed");
				mSelectedType = ObjectSelect::STATIC;
			}
		} 
		else  if(ih->IsKeyDown(OIS::KC_LCONTROL))
		{
			if (mCurrentDynamicIndex < mDynamicObjects.size()  - 1)
			{
				mCurrentDynamicIndex++;
			}
			if (mDynamicObjects.size() > 0)
			{
				mDynamicObjects[mCurrentDynamicIndex]->setMaterial("simpleRed");
				mSelectedType = ObjectSelect::DYNAMIC;
			}
		}
	}
	
	if (ih->KeyPressedThisFrame(OIS::KC_EQUALS) && !ih->IsKeyDown(OIS::KC_LSHIFT))
	{
		mPlacementScale += 1;
		mHUD->setScore((int) mPlacementScale);
	}
		if (ih->KeyPressedThisFrame(OIS::KC_EQUALS) && ih->IsKeyDown(OIS::KC_LSHIFT))
	{
		mPlacementScale += 10;
		mHUD->setScore((int) mPlacementScale);
	}
	if (ih->KeyPressedThisFrame(OIS::KC_MINUS) && !ih->IsKeyDown(OIS::KC_LSHIFT))
	{
		mPlacementScale -= 1;
		mHUD->setScore((int) mPlacementScale);
	}
		if (ih->KeyPressedThisFrame(OIS::KC_MINUS) && ih->IsKeyDown(OIS::KC_LSHIFT))
	{
		mPlacementScale -= 10;
		mHUD->setScore((int) mPlacementScale);
	}


	if (ih->KeyPressedThisFrame(OIS::KC_2) && !ih->IsKeyDown(OIS::KC_LSHIFT))

	{
		mHUD->setDebug(mPlayer->getPosition(), "Wall Placed");

		GameObject *wall = (new GameObject(GameObject::GATE, "Iceberg1.mesh",  mSceneManager,mPlayer->getPosition(), mPlayer->getOrientation()));
		wall->setScale(mPlacementScale);
		mStaticObjects.push_back(wall);
		mCurrentEditObject = wall;
	}

	if (ih->KeyPressedThisFrame(OIS::KC_3) && !ih->IsKeyDown(OIS::KC_LSHIFT))

	{
		mHUD->setDebug(mPlayer->getPosition(), "Target Placed");

		Ogre::Vector3 pos =  mPlayer->getPosition();
		pos.y = 15;
		GameObject *target = (new GameObject(GameObject::GATE, "Target.mesh",  mSceneManager,pos, mPlayer->getOrientation()));
		target->setScale(20);

		mDynamicObjects.push_back(target);
	}
	if (ih->KeyPressedThisFrame(OIS::KC_2) && ih->IsKeyDown(OIS::KC_LSHIFT))
	{
		if (mDynamicObjects.size() > 0)
		{
			GameObject *ob = mDynamicObjects[mDynamicObjects.size() - 1];
			mDynamicObjects.pop_back();
			delete ob;
		}
	}
	for (std::vector<GameObject *>::iterator it = mDynamicObjects.begin(); it != mDynamicObjects.end(); it++)
	{

		(*it)->yaw(Ogre::Degree(time * 90));
	}

}


void World::Think(float time)
{

	if (mEditing)
	{
		doEdit(time);
		return;
	}

	if (!mGameRunning)
	{
		return;
	}
	mPlayer->translate(mPlayer->getVelocity() * time);

	mCurrentTime += time;

	mHUD->setTime(mCurrentTime);

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



	InputHandler *ih = InputHandler::getInstance();

	if (ih->IsKeyDown(OIS::KC_O))
	{
		mDisp -= time ;
		mHUD->setDebug(mDisp, "Displacement");

	}
	if (ih->IsKeyDown(OIS::KC_L))
	{
		mDisp += time ;
		mHUD->setDebug(mDisp, "Displacement");
	}


	// Push the water down to cause a wake, but only if the boat is moving ...
	if( Math::Abs(mPlayer->getSpeed()) > 4)
	{

		Ogre::Vector3 boatBoxMin = mPlayer->minPointLocalScaled();
		Ogre::Vector3 boatBoxMax = mPlayer->maxPointLocalScaled();
		Ogre::Vector3 boatFacing = mPlayer->getFacing();
		Ogre::Vector3 front = mPlayer->getPosition() + mPlayer->getFacing() * boatBoxMax.z;
		Ogre::Vector3 back = mPlayer->getPosition() + mPlayer->getFacing() * boatBoxMin.z;


		float WaterPlaneXFront = front.x;
		float WaterPlaneXBack = back.x;
		float WaterPlaneZFront = front.z;
		float WaterPlaneZBack = back.z;

		mWaterMesh->push(WaterPlaneXFront * (float) COMPLEXITY / (float)PLANE_SIZE,WaterPlaneZFront * (float) COMPLEXITY / (float) PLANE_SIZE, -0.14f, false);
		mWaterMesh->push(WaterPlaneXBack * COMPLEXITY / PLANE_SIZE,WaterPlaneZBack * COMPLEXITY / PLANE_SIZE, 0.14f, false);

	}


	mWaterMesh->updateMesh(time);

	if (mPlayer->collides(mGoals[0]))
	{
		mCurrentIndex++;
		if (mCurrentIndex >= (int) mGoalPositions.size())
		{
			mCurrentIndex = 0;
		}

		for (unsigned int i = 1; i < mNumGoalsToShow; i++)
		{
			mGoals[i-1]->setPosition(mGoals[i]->getPosition());
			mGoals[i-1]->setOrientation(mGoals[i]->getOrientation());
		}
		mGoals[mNumGoalsToShow - 1]->setPosition( mGoalPositions[(mCurrentIndex+mNumGoalsToShow - 1) % mGoalPositions.size()]);
				mGoals[mNumGoalsToShow - 1]->setOrientation( mGoalOrientations[(mCurrentIndex+mNumGoalsToShow - 1) % mGoalPositions.size()]);

	}

	Ogre::Vector3 laserStart;
	Ogre::Vector3 laserDirection;

	if (mPlayer->isFiringLaser())
	{
		mPlayer->getLaser(laserStart, laserDirection);
		mAIManager->rayCollision(laserStart,laserDirection, dist, time * mPlayer->getLaserDPS());
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



	mArrowNode->pitch(Ogre::Degree(90));
	mArrowNode->yaw(Ogre::Degree(90));


}