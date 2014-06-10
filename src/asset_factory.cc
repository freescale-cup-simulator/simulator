#include <asset_factory.h>

AssetFactory::AssetFactory(dWorldID world, dSpaceID space,
                           TrimeshDataManager * trimesh_manager)
    : m_world(world)
    , m_space(space)
    , m_trimesh_manager(trimesh_manager)
{
}

Asset *AssetFactory::createAsset(Flags flags, const QString & mesh_name,
                                 GeomFunction geometry_func)
{
    dTriMeshDataID tid = nullptr;
    dGeomID gid = nullptr;
    dBodyID bid = nullptr;
    Ogre::SceneNode * node3d = nullptr;

    auto sm = m_rendering_objects->scene_manager;

    if (flags.testFlag(MeshGeometry))
    {
        tid = m_trimesh_manager->value(mesh_name);
        gid = dCreateTriMesh(m_space, tid, 0, 0, 0);
    }
    else if (flags.testFlag(CustomGeometry) && geometry_func)
    {
        gid = geometry_func(m_world,m_space);
    }

    if (flags.testFlag(PhyBody))
    {
        bid = dBodyCreate(m_world);
    }

    if (gid)
        dGeomSetBody(gid, bid);

    if (flags.testFlag(SceneNode))
    {
        m_rendering_objects->gl_context->makeCurrent(m_rendering_objects->window);

        node3d = sm->getRootSceneNode()->createChildSceneNode();
        node3d->setVisible(false, true);

        if (flags.testFlag(Body3D))
        {
            Ogre::Entity * ent = sm->createEntity(mesh_name.toStdString().c_str());
            node3d->attachObject(ent);
        }
        m_rendering_objects->gl_context->doneCurrent();
    }

    return new Asset(node3d, bid, gid);
}
