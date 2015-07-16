
#include "Player.h"
#include "World.h"
#include "Kinect.h"
#include "Achievements.h"
#include "InputHandler.h"


Player::Player(World *world, Kinect *k, Achievements *ach) : 
	GameObject(GameObject::PLAYER), mWorld(world), mKinect(k), mAchievements(ach)
{

	loadModel("Boat1.mesh", mWorld->SceneManager());
	setScale(10);
	setPosition(Ogre::Vector3(1500,0,1500));
	mVelocityDirection = Ogre::Vector3::UNIT_Z;
	mRollAngle = 0;
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
	mMaxSpeed = 1000;
	mSpeed = 0;
}

void Player::Think(float time)
{
	Ogre::Degree leftRight, frontBack;
	roll(-mRollAngle);

	updateAnglesFromControls(leftRight,frontBack);

	mSpeed +=frontBack.valueDegrees() * time * -10;
	if (mSpeed > mMaxSpeed)
	{
		mSpeed = mMaxSpeed;
	}
	if (mSpeed < -mMaxSpeed)
	{
		mSpeed = -mMaxSpeed;
	}



	if (Ogre::Math::Abs(leftRight) > Ogre::Degree(10))
	{
		Ogre::Degree change = leftRight * time * - mDegreesPerSecond / 30.0f;
		yaw(change);
	}
	mVelocityDirection = mFacing;
	mRollAngle = leftRight;
	roll(mRollAngle);

}


void Player::SendData(float time)
{


}

void Player::updateAnglesFromControls(Ogre::Degree &angle, Ogre::Degree &angle2)
{
	if (mEnableKinect)
	{
		angle =  mKinect->leftRightAngle() * mKinectSensitivityLR;
		angle2 = mKinect->frontBackAngle() * 0.8f *  mKinectSensitivityFB;
	}


	if (mEnableKeyboard)
	{

		if (InputHandler::getInstance()->IsKeyDown(OIS::KC_LEFT))
		{
//			roll(Ogre::Degree(1));
			angle = -Ogre::Degree(30);
		} 
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_RIGHT))
		{
//			roll(Ogre::Degree(-1));
			angle = Ogre::Degree(30);

		}
		if (InputHandler::getInstance()->IsKeyDown(OIS::KC_UP))
		{
//			yaw(Ogre::Degree(1));
			angle2 = Ogre::Degree(-30);
		}
		else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_DOWN))
		{
//			yaw(Ogre::Degree(-1));
			angle2 = Ogre::Degree(30);
		}
		else
		{
			angle2 = Ogre::Degree(0);
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