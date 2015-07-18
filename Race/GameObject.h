#pragma once

#include "OgreString.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"

class OBB;

namespace Ogre
{
    class SceneManager;
    class SceneNode;
}
class GameObject
{
public:

	enum ObjectType {PLAYER, STATIC_OBJECT, PROJECTILE, ENEMY, GATE};

    GameObject(ObjectType type);
	GameObject::GameObject(ObjectType typ, Ogre::String modelName,  Ogre::SceneManager *sm,Ogre::Vector3 position, Ogre::Quaternion orientation);

    ~GameObject(void);

	ObjectType type() {return mType;}

    void loadModel(Ogre::String modelName, Ogre::SceneManager *sm);
    void setPosition(Ogre::Vector3 newPosition);
    void setOrientation(Ogre::Quaternion newOrientation);
    void yaw(Ogre::Degree d);
    void pitch(Ogre::Degree d);
    void roll(Ogre::Degree d);

    void yaw(Ogre::Radian r);
    void pitch(Ogre::Radian r);
    void roll(Ogre::Radian r);

    void setScale(float newScale);

    void setScale(Ogre::Vector3 newScale);

	void setAlpha(float alpha);

	void translate(Ogre::Vector3 delta);


    bool collides(GameObject *other);

    bool collides(GameObject *other, Ogre::Vector3 &MTD);
    bool collides(const GameObject &other, Ogre::Vector3 &MTD);

	bool collides(Ogre::Vector3 start, Ogre::Vector3 direction, float &collidePos);

	Ogre::Vector3 minPointLocalScaled();
	Ogre::Vector3 maxPointLocalScaled();

    Ogre::Vector3 getPosition() { return mPosition; }
    Ogre::Quaternion getOrientation();
    Ogre::Vector3 getScale() { return mScale;}
	Ogre::Vector3 getFacing() {return mFacing;} 

	void setMaterial(Ogre::String materialName);
	void restoreOriginalMaterial();
	Ogre::SceneNode *getSceneNode() { return mSceneNode; }

protected:
	ObjectType mType;
    Ogre::SceneNode *mSceneNode;
    OBB *mCollision;
    Ogre::Vector3 mPosition;
    Ogre::SceneManager *mSceneManager;
    Ogre::Vector3 mScale;
	Ogre::Vector3 mMinPointLocal;
	Ogre::Vector3 mMaxPointLocal;
	Ogre::Vector3 mFacing;
	Ogre::Entity *mEntity;
	Ogre::String mMaterialName;
};

