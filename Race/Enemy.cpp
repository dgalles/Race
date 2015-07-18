#include "Enemy.h"


Enemy::Enemy(void) : GameObject(ENEMY)
{
}


Enemy::~Enemy(void)
{
}


void Target::Think(float time)
{
	yaw(Ogre::Degree(time * 90));

}