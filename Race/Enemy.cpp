#include "Enemy.h"
#include "Sound.h"
//////////////////////////////////////////////////////////////////////
////  Enemy Superclass
//////////////////////////////////////////////////////////////////////

Enemy::Enemy(void) : GameObject(ENEMY)
{
}

Enemy::~Enemy(void)
{
}

//////////////////////////////////////////////////////////////////////
////  Target 
//////////////////////////////////////////////////////////////////////


Target::Target(float initialHealth /* = 1 */) : Enemy()
{
	mHitLastFrame = false;
	mHitThisFrame = false;
	mHealth = initialHealth;
	mDead = false;
	mPlayingHitSound = false;
	mPlayingHandle = -1;
}

Target::~Target()
{
	if (mPlayingHitSound && mPlayingHandle >= 0)
	{
		SoundBank::getInstance()->stopManual(mPlayingHandle);
	}


}
void Target::Think(float time)
{
	yaw(Ogre::Degree(time * 90));

	if (mHitThisFrame && !mPlayingHitSound)
	{
		mPlayingHandle = SoundBank::getInstance()->fadeInManual("fire",10,true);
		mPlayingHitSound  = true;
	}
	if (mHitThisFrame && !mHitLastFrame)
	{
		setMaterial("HitMaterial");
	}
	if (!mHitThisFrame && mHitLastFrame)
	{
		restoreOriginalMaterial();
	}

	if (!mHitThisFrame && mPlayingHitSound && mPlayingHandle >= 0)
	{
		SoundBank::getInstance()->stopManual(mPlayingHandle);
		mPlayingHitSound  = false;
	}
	mHitLastFrame = mHitThisFrame;
	mHitThisFrame = false;

}




void Target::decreaseHealth(float amount)
{
	mHitThisFrame = true;
	mHealth -= amount;
	if (mHealth <= 0)
	{
		mHealth = 0;
		mDead = true;
	}
}
