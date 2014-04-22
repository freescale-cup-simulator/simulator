#include <asset.h>

Asset::Asset(Ogre::SceneNode *scene_node, dBodyID phy_body, dGeomID phy_geometry)
    : node3d(scene_node)
    , body(phy_body)
    , geometry(phy_geometry)
    , mesh(nullptr)
    , has_contact(false)
{
    if (node3d && node3d->numAttachedObjects())
    {
        auto ao = node3d->getAttachedObject(0);
        if (ao && ao->getMovableType() == "Entity")
            this->mesh = dynamic_cast<Ogre::Entity *>(ao)->getMesh().getPointer();
    }

    dMassSetZero(&mass);

    if (phy_body)
    {
        dBodySetData(phy_body, static_cast<void *>(this));
        dBodySetMovedCallback(phy_body, &Asset::bodyMovedCallback);
    }
}

Asset::~Asset()
{
    if (body)
        dBodyDestroy(body);

    if (geometry)
        dGeomDestroy(geometry);
    if (node3d)
    {
        // TODO: if not entity?
        // attached entity cleanup
        Ogre::SceneNode::ObjectIterator it = node3d->getAttachedObjectIterator();
        while (it.hasMoreElements())
        {
            Ogre::MovableObject* obj = static_cast<Ogre::MovableObject*>(it.getNext());
            node3d->getCreator()->destroyMovableObject(obj);
        }
        node3d->removeAndDestroyAllChildren();
        node3d->getCreator()->destroySceneNode(node3d);
    }
}

void Asset::rotate(const dQuaternion &rot)
{
    if (body)
        dBodySetQuaternion(body, rot);
    else if (geometry)
        dGeomSetQuaternion(geometry,rot);

    if(node3d)
        node3d->rotate(Ogre::Quaternion(rot[0], rot[1], rot[2], rot[3]));
}

void Asset::rotate(const Ogre::Quaternion &rot)
{
    rotate({rot.w, rot.x, rot.y, rot.z});
}

void Asset::setPosition(dReal x, dReal y, dReal z)
{
    if(body)
        dBodySetPosition(body, x, y, z);
    else if (geometry)
        dGeomSetPosition(geometry, x, y, z);

    if(node3d)
    {
        node3d->setPosition(x, y, z);
        node3d->_updateBounds();
    }
}

void Asset::setPosition(const Ogre::Vector3 &v)
{
    setPosition(v.x, v.y, v.z);
}

void Asset::setMass(dReal m)
{
    Q_ASSERT(body);
    Q_ASSERT(geometry);

    switch(dGeomGetClass(geometry))
    {
    case dTriMeshClass:
        dMassSetTrimeshTotal(&mass, m, geometry);
        dGeomSetPosition(geometry, -mass.c[0], -mass.c[1], -mass.c[2]);
        dMassTranslate(&mass, -mass.c[0], -mass.c[1], -mass.c[2]);
        dBodySetMass(body, &mass);
        break;
    case dSphereClass:
        dMassSetSphereTotal(&mass, m, dGeomSphereGetRadius(geometry));
        dBodySetMass(body, &mass);
        break;
    }
}

void Asset::setVisible(bool visible)
{
    if (node3d)
        node3d->setVisible(visible, true);
}

void Asset::bodyMoved()
{
    if (node3d)
    {
        const dReal * p = dBodyGetPosition(body);
        node3d->setPosition(p[0], p[1], p[2]);
        node3d->rotate(util::dq2oq(dBodyGetQuaternion(body)));
        node3d->_updateBounds();
    }

    if (geometry && dGeomGetClass(geometry) == dTriMeshClass)
        setTrimeshLastTransform(geometry);
}

void Asset::bodyMovedCallback(dBodyID body)
{
    reinterpret_cast<Asset *>(dBodyGetData(body))->bodyMoved();
}
