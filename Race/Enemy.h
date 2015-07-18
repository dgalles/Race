#pragma once

#include "GameObject.h"

class Enemy : public GameObject
{
public:
	Enemy(void);
	~Enemy(void);

	virtual void Think(float time) = 0;
protected:
	
};


class Target : public Enemy
{

public:
	virtual void Think(float time);


protected:




};
