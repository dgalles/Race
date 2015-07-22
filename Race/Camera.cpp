#include "OgreCamera.h"
#include "Camera.h"
#include "World.h"
#include "OgreVector3.h"
#include "Player.h"
#include "GameObject.h"
#include "InputHandler.h"

RaceCamera::RaceCamera(Ogre::Camera *renderCamera) :
mRenderCamera(renderCamera), mCurrentTrackingObject(0), mReview(false)
{
	mRenderCamera->setNearClipDistance(2);
	mType = FollowType::NORMAL;

	mDesiredFollowDistance = 200;
	mDesiredFollowHeight = 50;

	mCurrentFollowDistance = mDesiredFollowDistance;
	mCurrentFollowHeight = mDesiredFollowHeight;

	mOrbitDegree = 0;
	mShakeTime = 0;
	mAllowCameraMovement = true;
	// Any other initialization that needs to happen
}


void
	RaceCamera::SetFollowType(FollowType typ)
{
	if (typ == FollowType::NORMAL)
	{
		mDesiredFollowDistance = 100;
		mDesiredFollowHeight = 25;

	}
	else if (typ == FollowType::CLOSE)
	{
		mDesiredFollowDistance = 50;
		mDesiredFollowHeight = 15;

	}
	else if (typ = FollowType::HIGH)
	{
		mDesiredFollowDistance = 150;
		mDesiredFollowHeight = 80;
	}
}


void 
RaceCamera::Pause()
{
	mPaused = true;
}
void 
RaceCamera::UnPause()
{
	mPaused = false;
}

void
RaceCamera::TrackObject(GameObject *objectToTrack)
{
	mCurrentTrackingObject = objectToTrack;

}

void RaceCamera::Shake(float time)
{
	mShakeTime = time;
	mShakeRoll = 0;
	mShakePitch = 0;
	mShakeOffset = Ogre::Vector3::ZERO;
}


void
RaceCamera::Think(float time)
{


	if (mAllowCameraMovement)
	{
		InputHandler *ih = InputHandler::getInstance();

		if (ih->IsKeyDown(OIS::KC_NUMPAD4))
		{
			mOrbitDegree += time * 20;

		}
		if (ih->IsKeyDown(OIS::KC_NUMPAD6))
		{
			mOrbitDegree -= time * 20;
		}
		float mult = time * 100;
		if (ih->IsKeyDown(OIS::KC_LSHIFT))
		{
			mult *= 10;
		}


		if (ih->IsKeyDown(OIS::KC_NUMPAD8) && ih->IsKeyDown(OIS::KC_RSHIFT))
		{
			mCurrentFollowDistance += mult;
		}
		if (ih->IsKeyDown(OIS::KC_NUMPAD2) && ih->IsKeyDown(OIS::KC_RSHIFT))
		{
			mCurrentFollowDistance -= mult;
		}
		if (ih->IsKeyDown(OIS::KC_NUMPAD8) && !ih->IsKeyDown(OIS::KC_RSHIFT))
		{
			mCurrentFollowHeight += mult;
		}
		if (ih->IsKeyDown(OIS::KC_NUMPAD2) && !ih->IsKeyDown(OIS::KC_RSHIFT))
		{
			mCurrentFollowHeight -= mult;
		}


		if (ih->IsKeyDown(OIS::KC_NUMPAD5))
		{
			mCurrentFollowDistance = 200;
			mCurrentFollowHeight = 50;
			mOrbitDegree = 0;

		}

	}

	if (mCurrentTrackingObject != 0)
	{

		Ogre::Vector3 trackPos = mCurrentTrackingObject->getPosition();
		Ogre::Vector3 cameraPos;


		float cameraSpeed = 100;

		Ogre::Vector3 delta = mCurrentTrackingObject->getFacing() * -mCurrentFollowDistance;

		if (mOrbitDegree != 0)
		{

		Ogre::Quaternion rot;
		rot.FromAngleAxis(Ogre::Degree(mOrbitDegree),Ogre::Vector3::UNIT_Y);

		delta = rot * delta;
		}

		delta.y = mCurrentFollowHeight;

		

		mRenderCamera->setPosition(trackPos + delta);
		mRenderCamera->lookAt(trackPos);

		if (mShakeTime >= time)
		{
			mShakeTime -= time;
			float RollDegreesPerSecond = 60;
			float PitchDegreesPerSecond = 60;
			float rTest1 = ((rand() / ((float) RAND_MAX / 2))  - 1);
			float rTest2 = ((rand() / ((float) RAND_MAX / 2))  - 1);
			mShakeRoll += rTest1 * time * RollDegreesPerSecond;
			mShakePitch += rTest2 * time * PitchDegreesPerSecond;
			mRenderCamera->roll(Ogre::Degree(mShakeRoll));
			mRenderCamera->pitch(Ogre::Degree(mShakePitch));
			mRenderCamera->moveRelative(mShakeOffset);
		}
		else
		{
			mShakeTime = 0;
		}

	}

	// Any code needed here to move the camera about per frame
	//  (use mRenderCamera to get the actual render camera, of course!)
}
