#pragma once

#include "GameObject.h"

class Enemy : public GameObject
{
public:
	Enemy(void);
	~Enemy(void);
	virtual void Think(float time) { }
	virtual void decreaseHealth(float amount) { }
	virtual bool isDead() { return false; }
	virtual float getHealth() { return mHealth; }
	virtual float getValue() { return mValue; }
	virtual void setValue(float val) { mValue = val;} 
protected:

	float mHealth;
	float mValue;
	
};


class Target : public Enemy
{

public:
	Target(float initialHealth = 1.0f);
	virtual void Think(float time);
	virtual void decreaseHealth(float amount);
	virtual bool isDead() { return mDead; }
	virtual float getHealth() { return mHealth; }

	virtual ~Target();

protected:

	bool mHitThisFrame;
	bool mHitLastFrame;
	bool mDead;
	bool mPlayingHitSound;
	int mPlayingHandle;

};
