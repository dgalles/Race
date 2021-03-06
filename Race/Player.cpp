#define NOMINMAX

#include "Player.h"
#include "World.h"
#include "Kinect_USF.h"
#include "Achievements.h"
#include "InputHandler.h"
#include "HUD.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"
#include "Sound.h"

#include "Camera.h"

void Player::createLaserMesh(void)
{
	//declare all of our grass variables
	const float width = 3.0f;
	const float length = 10.0f;

	Ogre::ManualObject laser("LaserObject");

	Ogre::Vector3 vec(width / 2, 0, 0);
	Ogre::Quaternion rot;
	rot.FromAngleAxis(Ogre::Degree(60), Ogre::Vector3::UNIT_Z);

	//start defining our manual object
	laser.begin("LaserMaterial2", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	//define the 4 vertices of our quad and set to the texture coordinates
	for(int i = 0; i < 6; ++i)
	{
		laser.position(-vec.x, -vec.y, length);
		laser.textureCoord(0, 0);

		laser.position(vec.x, vec.y, length);
		laser.textureCoord(1, 0);

		laser.position(-vec.x, -vec.y,0);
		laser.textureCoord(0, 1);

		laser.position(vec.x, vec.y,0);
		laser.textureCoord(1, 1);

		int offset = i * 4;

		laser.triangle(offset, offset + 3, offset + 1);
		laser.triangle(offset, offset + 2, offset + 3);

		//rotate the next quad
		vec = rot * vec;
	}
	laser.end();

	//create an actual mesh out of this object
	laser.convertToMesh("LaserMesh");
}



Player::Player(World *world, Kinect *k, Achievements *ach) : 
	GameObject(GameObject::PLAYER), mWorld(world), mKinect(k), mAchievements(ach)
{

	mStopOnFiring = true;
	mNoTurnFire = true;
	mIsFiringLaser = true;
	mCurrentYPercent = 0;
	mCurrentXPercent = 0.5f;

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


	createLaserMesh();

	Ogre::Entity *laserEnt = sm->createEntity("LaserMesh");
	mLaserSceneNode = mBarrelSceneNode->createChildSceneNode();
	mLaserSceneNode->setPosition(Ogre::Vector3(0,0,	barrelEnt->getBoundingBox().getMaximum().z));
	mLaserSceneNode->attachObject(laserEnt);
	mLaserSceneNode->setScale(0.1f,0.1f,10.0f);


	mTurretScaleY = 0.0f;


	mBarrelSceneNode->setScale(1,1,mTurretScaleY);
	mTurretSceneNode->setScale(1,mTurretScaleY,1);
	mLaserSoundHandle = -1;

	reset();
}




void Player::reset()
{
	mInvertControls = false;
	mEnableKeyboard = false;
	mEnableKinect = true;
	mKinectSensitivityFB = 1.0f;
	mKinectSensitivityLR = 1.0f;
	mDegreesPerSecond = 45.0f;
	mMaxSpeed = 5;
	mSpeed = 0;

	mAccel = 10;
	mLaserRechargeTime = 2;
	mLaserTime = 10;
	mLaserPercentage = 1;
	mDesiredlaserLength = 500;
	mLaserNeedsRecharging = false;
	mLaserDPS = 1.0f;
	mLaserAllowed = true;
	mAimVertical = true;

}


void Player::SetLaserLength(float length)
{
	mDesiredlaserLength = length;
	SetLaserLengthInternal(length);
}

void Player::SetLaserLengthInternal(float length)
{
		mLaserSceneNode->setScale(0.1f, 0.1f, length / 100.0f);

}
void Player::Think(float time)
{
	Ogre::Degree leftRight, frontBack;
	roll(-mRollAngle);

	float turretDelta = 0.2f;

	float xPercent, yPercent;

	updateAnglesFromControls(leftRight,frontBack, xPercent, yPercent, time);

	if (yPercent >turretDelta && mLaserAllowed)
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
			mTurretSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);

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
			mTurretSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);

		}
	}

	if (mStopOnFiring && mTurretScaleY > 0)
	{
		mSpeed = 0;
	}

	mSpeed +=frontBack.valueDegrees() * time * -mAccel;
	if (mSpeed > mMaxSpeed * 100)
	{
		mSpeed = mMaxSpeed* 100;
	}
	if (mSpeed < -mMaxSpeed* 100)
	{
		mSpeed = -mMaxSpeed* 100;
	}

	if (mTurretScaleY == 1.0f)
	{


		mTurretSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
		mTurretSceneNode->roll(Ogre::Degree(-leftRight));
		mTurretSceneNode->yaw(Ogre::Degree((xPercent -0.5f) * -90));
		mBarrelSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
		if (mAimVertical)
		{
			mBarrelSceneNode->pitch(Ogre::Degree((yPercent -turretDelta - 0.1f) * -30));
		}

	}

	if (mTurretScaleY == 1.0f && mLaserPercentage > 0.0f && !mLaserNeedsRecharging)
	{
		SetLaserLengthInternal (mDesiredlaserLength);
		mLaserPercentage -= time / mLaserTime;
		if (mLaserPercentage < 0)
		{
			mLaserPercentage = 0;
			mLaserNeedsRecharging = true;
		}
		mWorld->getHUD()->setLaserPower(mLaserPercentage, mLaserNeedsRecharging);

		mLaserSceneNode->roll(Ogre::Degree(time *180));

		if (!mIsFiringLaser)
		{
			mLaserSoundHandle = SoundBank::getInstance()->fadeInManual("laser",10,true);
			mIsFiringLaser = true;
		}
	}
	else
	{
		SetLaserLengthInternal(0);
		mLaserPercentage += time / mLaserRechargeTime;
		if (mLaserPercentage > 1)
		{
			mLaserPercentage = 1;
			mLaserNeedsRecharging = false;
		}
		mWorld->getHUD()->setLaserPower(mLaserPercentage,mLaserNeedsRecharging);
		if (mIsFiringLaser && mLaserSoundHandle >= 0)
		{
			SoundBank::getInstance()->fadeOutManual(mLaserSoundHandle,10);
			mLaserSoundHandle = -1;
		}
		mIsFiringLaser = false;

	}

	mWorld->getHUD()->setSpeed(mSpeed / 100.0f);


	if (Ogre::Math::Abs(leftRight) > Ogre::Degree(10) && (mTurretScaleY == 0.0f || !mNoTurnFire))
	{
		Ogre::Degree change = leftRight * time * - mDegreesPerSecond / 30.0f;
		yaw(change);
	}
	mVelocityDirection = mFacing;
	mRollAngle = leftRight;
	roll(mRollAngle);

}

void Player::getLaser(Ogre::Vector3 &startPos, Ogre::Vector3 &direction)
{
	startPos = mLaserSceneNode->_getDerivedPosition();
	direction = mLaserSceneNode->_getDerivedOrientation() * Ogre::Vector3::UNIT_Z;
}

void Player::setOrientation(Ogre::Quaternion newOrientation)
{
	mRollAngle = 0.0f;
	GameObject::setOrientation(newOrientation);
}

void Player::SendData(float time)
{




}

void Player::updateAnglesFromControls(Ogre::Degree &angle, Ogre::Degree &angle2, float &xPercent, float &yPercent, float time)
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
		if (InputHandler::getInstance()->IsKeyDown(OIS::KC_W))
		{
			mCurrentYPercent += time;
			if (mCurrentYPercent > 1)
				mCurrentYPercent = 1;

		}
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_S))
		{
			mCurrentYPercent -= time;
			if (mCurrentYPercent < 0)
				mCurrentYPercent = 0;
		}
		if (InputHandler::getInstance()->IsKeyDown(OIS::KC_A))
		{
			mCurrentXPercent -= time;
			if (mCurrentXPercent < 0)
				mCurrentXPercent = 0;

		}
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_D))
		{
			mCurrentXPercent += time;
			if (mCurrentXPercent > 1)
				mCurrentXPercent = 1;
		}
		xPercent = mCurrentXPercent;
		yPercent = mCurrentYPercent;


	}
	//if (!mUseFrontBack)
	//{
	//	angle2 = Ogre::Degree(0);
	//}
	//if (mInvertControls)
	//{
	//	angle2 = -angle2;
	//}
}