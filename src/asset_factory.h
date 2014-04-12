#ifndef ASSET_FACTORY_H
#define ASSET_FACTORY_H

#include <ode/ode.h>
#include <OgreRoot.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreSubMesh.h>
#include <OGRE/OgreVertexIndexData.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreResourceGroupManager.h>

#include <QObject>
#include <QString>
#include <QHash>
#include <QVector>
#include <QQuickWindow>
#include <QOpenGLContext>

#include <functional>
#include <physics_simulation.h>
#include <global_renderer.h>
#include <trimesh_data_manager.h>
#include <asset.h>

class AssetFactory
{
public:
    enum CreateAssetFlags{
        SceneNode=1,
        Body3D=2,
        MeshGeometry=4,
        PhyBody=8,
        CustomGeometry=16
    };
    typedef std::function<dGeomID(dWorldID,dSpaceID)> GeomFunction;
    AssetFactory(dWorldID world, dSpaceID space, TrimeshDataManager * trimesh_manager);

    Asset * createAsset(int flags,const QString & mesh_name, GeomFunction geometry_func=nullptr);

    inline void setRenderingInstances(GlobalRenderer::RenderingInstances * i)
    {
        m_rendering_instances = i;
    }

private:

    dWorldID m_world;
    dSpaceID m_space;
    TrimeshDataManager * m_trimesh_manager;

    GlobalRenderer::RenderingInstances * m_rendering_instances;
};

#endif // ASSET_FACTORY_H
