#include "Enemy.h"
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
}


void Target::Think(float time)
{
	yaw(Ogre::Degree(time * 90));
	if (mHitThisFrame && !mHitLastFrame)
	{
		setMaterial("HitMaterial");
	}
	if (!mHitThisFrame && mHitLastFrame)
	{
		restoreOriginalMaterial();
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
