#ifndef ASSET_H
#define ASSET_H

#include <OgreRoot.h>
#include <OgreMovableObject.h>
#include <OgreEntity.h>
#include <ode/ode.h>

class Asset
{
public:
    Asset(Ogre::SceneNode * scene_node, dBodyID phy_body, dGeomID phy_geometry);
    ~Asset();

    bool isDirty();
    void rotate(const dQuaternion & rot);
    void rotate(const Ogre::Quaternion & rot);
    void setPosition(dReal x, dReal y, dReal z);
    void setMass(dReal m);
    void setVisible(bool visible);

    Ogre::SceneNode * node3d;
    dBodyID body;
    dGeomID geometry;
    dMass mass;
    Ogre::Mesh * mesh;
    bool m_is_dirty;
};

#endif // ASSET_H
