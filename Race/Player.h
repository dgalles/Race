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

protected:
	void SendData(float time);
	void updateAnglesFromControls(Ogre::Degree &angle, Ogre::Degree &angle2);


	World *mWorld;
	Kinect *mKinect;

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

};