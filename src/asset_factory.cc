#include <asset_factory.h>

AssetFactory::AssetFactory(dWorldID world,
                            dSpaceID space, TrimeshDataManager * trimesh_manager)
    : m_world(world)
    , m_space(space)
    , m_trimesh_manager(trimesh_manager)
{
}
/*
 * enum CreateAssetFlags{
        Body3D=1,
        MeshGeometry=2,
        PhyBody=4,
        CustomGeometry=8
    };
 *
*/
Asset *AssetFactory::createAsset(int flags, const QString & mesh_name, GeomFunction geometry_func)
{
    dTriMeshDataID tid=0;
    dGeomID gid=0;
    dBodyID bid=0;
    Ogre::SceneNode * node3d=0;
    if (flags & MeshGeometry)
    {
        tid=m_trimesh_manager->value(mesh_name);
        gid = dCreateTriMesh(m_space, tid, 0, 0, 0);
    }
    else
        if ((flags & CustomGeometry) && geometry_func)
            gid=geometry_func(m_world,m_space);
    if (flags & PhyBody)
        bid=dBodyCreate(m_world);
    if (gid!=0)
        dGeomSetBody(gid, bid);
    if (flags & Body3D)
    {
        m_rendering_instances->gl_context->makeCurrent(m_rendering_instances->surface);
        if(flags & SceneNode)
        {
            node3d=m_rendering_instances->scene_manager->getRootSceneNode()->createChildSceneNode();
            node3d->setVisible(false,true);
        }
        if((flags & SceneNode) && (flags & Body3D))
        {
            Ogre::Entity* ent=m_rendering_instances->scene_manager->createEntity(mesh_name.toStdString().c_str());
            node3d->attachObject(ent);
        }
        m_rendering_instances->gl_context->doneCurrent();
    }
    return new Asset(node3d,bid,gid);
}
