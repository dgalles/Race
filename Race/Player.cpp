
#include "Player.h"
#include "World.h"
#include "Kinect.h"
#include "Achievements.h"
#include "InputHandler.h"
#include "HUD.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"

#include "Camera.h"

Player::Player(World *world, Kinect *k, Achievements *ach) : 
	GameObject(GameObject::PLAYER), mWorld(world), mKinect(k), mAchievements(ach)
{

	loadModel("Boat1.mesh", mWorld->SceneManager());
	setScale(10);
	setPosition(Ogre::Vector3(1500,-3,1500));
	mVelocityDirection = Ogre::Vector3::UNIT_Z;
	mRollAngle = 0;

	Ogre::SceneManager *sm = mWorld->SceneManager();
    Ogre::Entity *turretEnt =sm->createEntity("TurretBase1.mesh");
	mTurretSceneNode = mSceneNode->createChildSceneNode();
	mTurretSceneNode->attachObject(turretEnt);
	mTurretSceneNode->setPosition(Ogre::Vector3(0,1.4f,1));

	Ogre::Entity *barrelEnt =sm->createEntity("Barrel1.mesh");
	mBarrelSceneNode = mTurretSceneNode->createChildSceneNode();
	mBarrelSceneNode->setPosition(Ogre::Vector3(0,0.3f,0));
	mBarrelSceneNode->attachObject(barrelEnt);
	mBarrelSceneNode->pitch(Ogre::Degree(-45));
	mTurretScaleY = 0.0f;

	mBarrelSceneNode->setScale(1,1,mTurretScaleY);
	mTurretSceneNode->setScale(1,mTurretScaleY,1);


	reset();
}


void Player::reset()
{
	mInvertControls = false;
	mEnableKeyboard = true;
	mEnableKinect = true;
	mKinectSensitivityFB = 1.0f;
	mKinectSensitivityLR = 1.0f;
	mDegreesPerSecond = 45.0f;
	mMaxSpeed = 500;
	mSpeed = 0;
}

void Player::Think(float time)
{
	Ogre::Degree leftRight, frontBack;
	roll(-mRollAngle);

	float xPercent, yPercent;

	updateAnglesFromControls(leftRight,frontBack, xPercent, yPercent);

	if (yPercent > 0.2f)
	{
		if (mTurretScaleY < 1.0f)
		{
			mTurretScaleY += time*4;
			if (mTurretScaleY > 1.0f)
			{
				mTurretScaleY = 1.0f;
			}
			mTurretSceneNode->setScale(1,mTurretScaleY,1);
			mBarrelSceneNode->setScale(1,1,mTurretScaleY);
		}
	}  else {
		if (mTurretScaleY > 0)
		{
			mTurretScaleY -= time*4;
			if (mTurretScaleY < 0.0f)
			{
				mTurretScaleY = 0.0f;
			}
			mTurretSceneNode->setScale(1,mTurretScaleY,1);
			mBarrelSceneNode->setScale(1,1,mTurretScaleY);
		}
	}


	if (mTurretScaleY == 1.0f)
	{
		mTurretSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
		mTurretSceneNode->yaw(Ogre::Degree((xPercent -0.5) * -90));
		mBarrelSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
		mBarrelSceneNode->pitch(Ogre::Degree(yPercent * -60));
	}

	mSpeed +=frontBack.valueDegrees() * time * -10;
	if (mSpeed > mMaxSpeed)
	{
		mSpeed = mMaxSpeed;
	}
	if (mSpeed < -mMaxSpeed)
	{
		mSpeed = -mMaxSpeed;
	}

	mWorld->getHUD()->setSpeed((int) mSpeed);


	if (Ogre::Math::Abs(leftRight) > Ogre::Degree(10))
	{
		Ogre::Degree change = leftRight * time * - mDegreesPerSecond / 30.0f;
		yaw(change);
	}
	mVelocityDirection = mFacing;
	mRollAngle = leftRight;
	roll(mRollAngle);

}


void Player::setOrientation(Ogre::Quaternion newOrientation)
{
	mRollAngle = 0.0f;
	GameObject::setOrientation(newOrientation);
}

void Player::SendData(float time)
{


}

void Player::updateAnglesFromControls(Ogre::Degree &angle, Ogre::Degree &angle2, float &xPercent, float &yPercent)
{
	if (mEnableKinect)
	{
		angle =  mKinect->leftRightAngle() * mKinectSensitivityLR;
		angle2 = mKinect->frontBackAngle() * 0.8f *  mKinectSensitivityFB;
		bool dummy;
		Ogre::Vector3 handPos = mKinect->handPositionAdjusted(dummy);
		xPercent = handPos.x;
		yPercent = handPos.y;
	}


	if (mEnableKeyboard)
	{

		if (InputHandler::getInstance()->IsKeyDown(OIS::KC_LEFT))
		{
			angle = -Ogre::Degree(30);
		} 
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_RIGHT))
		{
			angle = Ogre::Degree(30);

		}
		if (InputHandler::getInstance()->IsKeyDown(OIS::KC_UP))
		{
			angle2 = Ogre::Degree(-30);
		}
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_DOWN))
		{
			angle2 = Ogre::Degree(30);
		}
		else
		{
			angle2 = Ogre::Degree(0);
		}

		if (InputHandler::getInstance()->IsKeyDown(OIS::KC_Q))
		{
			mBarrelSceneNode->pitch(Ogre::Degree(0.1f));
		}
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_A))
		{
			mBarrelSceneNode->pitch(Ogre::Degree(-0.1f));
		}
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_W))
		{
			mTurretSceneNode->yaw(Ogre::Degree(0.1f));
		}
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_S))
		{
			mTurretSceneNode->yaw(Ogre::Degree(-0.1f));
		}
	
	}
	if (!mUseFrontBack)
	{
		angle2 = Ogre::Degree(0);
	}
	if (mInvertControls)
	{
		angle2 = -angle2;
	}
}