#include <asset.h>

Asset::Asset(Ogre::SceneNode *scene_node, dBodyID phy_body, dGeomID phy_geometry)
    : node3d(scene_node)
    , body(phy_body)
    , geometry(phy_geometry)
    , mesh(0)
{
    if(node3d)
    {
        if(node3d->numAttachedObjects()>0)
        {
            Ogre::MovableObject * mo=node3d->getAttachedObject(0);
            if(mo->getMovableType()=="Entity")
                this->mesh=(dynamic_cast<Ogre::Entity *>(mo))->getMesh().getPointer();
        }
    }
    dMassSetZero(&mass);
}

Asset::~Asset()
{
    if(body)
        dBodyDestroy(body);
    if(geometry)
        dGeomDestroy(geometry);
    if(node3d)
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

bool Asset::isDirty()
{
    return m_is_dirty;
}

void Asset::rotate(const dQuaternion &rot)
{
    if(body)
        dBodySetQuaternion(body,rot);
    else
        if(geometry)
            dGeomSetQuaternion(geometry,rot);
    if(node3d)
    {
        node3d->rotate(Ogre::Quaternion(rot[0],rot[1],rot[2],rot[3]));
        m_is_dirty=true;
    }
}

void Asset::rotate(const Ogre::Quaternion &rot)
{
    dQuaternion r={rot.w,rot.x,rot.y,rot.z};
    rotate(r);
}

void Asset::setPosition(dReal x, dReal y, dReal z)
{
    if(body)
        dBodySetPosition(body,x,y,z);
    else
        if(geometry)
            dGeomSetPosition(geometry,x,y,z);
    if(node3d)
    {
        node3d->setPosition(x,y,z);
        m_is_dirty=true;
    }
}

void Asset::setMass(dReal m)
{
    if(body)
    {
        dMassSetTrimeshTotal(&mass, m, geometry);
        dGeomSetPosition(geometry, -mass.c[0], -mass.c[1], -mass.c[2]);
        dMassTranslate(&mass, -mass.c[0], -mass.c[1], -mass.c[2]);
        dBodySetMass(body,&mass);
    }
}

void Asset::setVisible(bool visible)
{
    if(node3d)
        node3d->setVisible(visible,true);
}
