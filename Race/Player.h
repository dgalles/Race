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


	void SetLaserLength(float length);
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

	bool getLaserAllowed() { return mLaserAllowed; }
	void setLaserAllowed(bool allowed) { mLaserAllowed = allowed; }

	bool getLaserAimVertical() { return mAimVertical; }
	void setLaserAimVertical(bool aimVert) {  mAimVertical = aimVert; }


	void getLaser(Ogre::Vector3 &startPos, Ogre::Vector3 &direction);

	bool isFiringLaser() { return mIsFiringLaser; }

	float getLaserDPS() { return mLaserDPS; }
	void setLaserDPS(float newDPS) { mLaserDPS = newDPS;}

	void setLaserDuration(float dur) { mLaserTime = dur; }
	float getLaserDuration() { return mLaserTime; }

	void setLaserCooldown(float cool) { mLaserRechargeTime = cool; }
	float getLaserCooldown() { return mLaserRechargeTime; }

	float getAcceleration() { return mAccel; }
	void setAcceleration(float accel) { mAccel = accel; }

protected:
	void SendData(float time);
	void updateAnglesFromControls(Ogre::Degree &angle, Ogre::Degree &angle2, float &xPercent, float &yPercent, float time);
	void SetLaserLengthInternal(float length);
	void createLaserMesh(void);

	World *mWorld;
	Kinect *mKinect;
	bool mIsFiringLaser;

	float mAccel;

	bool mAimVertical;
	bool mLaserAllowed;

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

	float mLaserDPS;
	float mCurrentYPercent;
	float mCurrentXPercent;

	float mLaserRechargeTime;
	float mLaserTime;
	float mLaserPercentage;
	bool mLaserNeedsRecharging;
	float mDesiredlaserLength;
	float mTurretScaleY;

};