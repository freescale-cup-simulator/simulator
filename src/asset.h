#ifndef ASSET_H
#define ASSET_H

#include <QtDebug>

#include <OgreRoot.h>
#include <OgreMovableObject.h>
#include <OgreEntity.h>
#include <ode/ode.h>

class Asset
{
public:
    Asset(Ogre::SceneNode * scene_node, dBodyID phy_body, dGeomID phy_geometry);
    ~Asset();

    void rotate(const dQuaternion & rot);
    void rotate(const Ogre::Quaternion & rot);
    void setPosition(dReal x, dReal y, dReal z);
    void setPosition(const Ogre::Vector3 & v);
    void setMass(dReal m);
    void setVisible(bool visible);

    Ogre::SceneNode * node3d;
    dBodyID body;
    dGeomID geometry;
    dMass mass;
    Ogre::Mesh * mesh;

    bool has_contact;
    dContact contact;

    static void bodyMovedCallback(dBodyID body);
    void bodyMoved();
private:
    inline void setTrimeshLastTransform(dGeomID id)
    {
        const dReal * pos = dGeomGetPosition(id);
        const dReal * rot = dGeomGetRotation(id);
        dMatrix4 transform
        {
            rot[0], rot[4], rot[8],  0,
            rot[1], rot[5], rot[9],  0,
            rot[2], rot[6], rot[10], 0,
            pos[0], pos[1], pos[2],  1
        };
        dGeomTriMeshSetLastTransform(id, transform);
    }
};

#endif // ASSET_H
