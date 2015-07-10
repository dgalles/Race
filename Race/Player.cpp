
#include "Player.h"
#include "World.h"
#include "Kinect.h"
#include "Achievements.h"
#include "InputHandler.h"


Player::Player(World *world, Kinect *k, Achievements *ach) : 
	GameObject(GameObject::PLAYER), mWorld(world), mKinect(k), mAchievements(ach)
{

	loadModel("car.mesh", mWorld->SceneManager());
	setScale(Ogre::Vector3(2,3,5));
	setPosition(Ogre::Vector3(500,0,500));
	mVelocityDirection = Ogre::Vector3::UNIT_Z;
	reset();
}


void Player::reset()
{
	mInvertControls = false;
	mEnableKeyboard = true;
	mEnableKinect = true;
	mKinectSensitivityFB = 1.0f;
	mKinectSensitivityLR = 1.0f;
	mDegreesPerSecond = 20.0f;
	mSpeed = 10;
}

void Player::Think(float time)
{
	Ogre::Degree leftRight, frontBack;

	updateAnglesFromControls(leftRight,frontBack);

		mSpeed +=frontBack.valueDegrees() * time * -10;

	if (Ogre::Math::Abs(leftRight) > Ogre::Degree(10))
	{
		Ogre::Degree change = leftRight * time * - mDegreesPerSecond / 30.0f;
		yaw(change);
	}
	mVelocityDirection = mFacing;

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