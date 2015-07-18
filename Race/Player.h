#include "OgreVector3.h"
#include "Receivers.h"
#include "GameObject.h"
class Kinect;
class Achievements;
class World;





class Player : public GameObject
{

public:
	Player(World *world, Kinect *k, Achievements *ach);



	bool getEnableKinect() { return mEnableKinect; }
	void setEnableKinect(bool enable) { mEnableKinect = enable;}

	bool getEnableKeyboard() { return mEnableKeyboard; }
	void setEnableKeyboard(bool enable) { mEnableKeyboard = enable;}


	bool getInvertControls() { return mInvertControls; }
	void setInvertControls(bool invert) { mInvertControls = invert;}

	float getDegressPerSecond() { return mDegreesPerSecond;}
	void setDegreesPerSecond(float newDPS) { mDegreesPerSecond = newDPS; }
	void Think(float time);
	
	Ogre::Vector3 getVelocity() { return mVelocityDirection * mSpeed; }

	void setSpeed(float newSpeed) { mSpeed = newSpeed;} 
	float getSpeed() { return mSpeed;} 


	float getMaxSpeed() { return mMaxSpeed; }
	void setMaxSpeed(float newMaxSpeed) { mMaxSpeed = newMaxSpeed;}
	void reset();

	void setStopOnFiring(bool stopOnFiring) { mStopOnFiring = stopOnFiring;}
	bool getStopOnFiring() { return mStopOnFiring; }
	void setOrientation(Ogre::Quaternion newOrientation);

	void getLaser(Ogre::Vector3 &startPos, Ogre::Vector3 &direction);

	bool isFiringLaser() { return mIsFiringLaser; }


protected:
	void SendData(float time);
	void updateAnglesFromControls(Ogre::Degree &angle, Ogre::Degree &angle2, float &xPercent, float &yPercent, float time);

	void createLaserMesh(void);

	World *mWorld;
	Kinect *mKinect;
	bool mIsFiringLaser;


	bool mEnableKinect;
	bool mEnableKeyboard;
	Achievements *mAchievements;
	float mKinectSensitivityLR;
	float mKinectSensitivityFB;
	bool mUseFrontBack;
	bool mInvertControls;

	Ogre::Vector3 mVelocityDirection;
	float mSpeed;
	float mMaxSpeed;
	float mDegreesPerSecond;
	Ogre::Degree mRollAngle;
	Ogre::SceneNode *mBarrelSceneNode;
	Ogre::SceneNode *mTurretSceneNode;
	Ogre::SceneNode *mLaserSceneNode;
	bool mStopOnFiring;

	float mCurrentYPercent;
	float mCurrentXPercent;

	float mTurretScaleY;

};