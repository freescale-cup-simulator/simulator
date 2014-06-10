#ifndef ASSET_H
#define ASSET_H

#include <QtDebug>

#include <OgreRoot.h>
#include <OgreMovableObject.h>
#include <OgreEntity.h>
#include <ode/ode.h>

#include <util.h>

/*!
 * \brief Joins ODE bodies with Ogre scene nodes
 *
 * This class represents an entity in the simulated world, providing
 * a consistent interface for manipulating an Ogre SceneNode and ODE Body
 * and making sure their position/rotation are synchronized.
 */

class Asset
{
public:
    /*!
     * \brief Asset
     * \param scene_node optional graphical representation of this Asset; nullptr if unused.
     * \param phy_body optional ODE Body
     * \param phy_geometry optional ODE Geometry
     */
    Asset(Ogre::SceneNode * scene_node, dBodyID phy_body, dGeomID phy_geometry);
    ~Asset();

    void rotate(const dQuaternion & rot);
    void rotate(const Ogre::Quaternion & rot);

    void setPosition(dReal x, dReal y, dReal z);
    void setPosition(const Ogre::Vector3 & v);

    void setMass(dReal m);
    /*!
     * \brief if this asset has a graphical representation (SceneNode), change
     * it's visibility
     */
    void setVisible(bool visible);

    Ogre::SceneNode * node3d;
    dBodyID body;
    dGeomID geometry;
    dMass mass;
    Ogre::Mesh * mesh;

    /*!
     * \brief indicates whether the user has provided ODE contact parameters
     * for this mesh
     */
    bool has_contact;
    /*!
     * \brief dContact struct to be used when this asset collides with another
     */
    dContact contact;

    static void bodyMovedCallback(dBodyID body);
    /*!
     * \brief updates the position of this asset's SceneNode according to it's Body;
     * usually called via bodyMovedCallback by ODE during simulation
     */
    void bodyMoved();
private:
    /*!
     * \brief ODE-specific, required for trimesh collisions to work
     */
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
