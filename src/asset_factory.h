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
#include <QFlags>

#include <functional>
#include <physics_simulation.h>
#include <global_renderer.h>
#include <trimesh_data_manager.h>
#include <asset.h>

class AssetFactory : public RenderingObjectsUser
{
public:
    enum Flags_
    {
        SceneNode = 1,
        Body3D = 2,
        MeshGeometry = 4,
        PhyBody = 8,
        CustomGeometry = 16
    };
    Q_DECLARE_FLAGS(Flags, Flags_)

    typedef std::function<dGeomID (dWorldID, dSpaceID)> GeomFunction;
    AssetFactory(dWorldID world, dSpaceID space, TrimeshDataManager * trimesh_manager);

    Asset * createAsset(Flags flags, const QString & mesh_name = QString(),
                        GeomFunction geometry_func = nullptr);

private:

    dWorldID m_world;
    dSpaceID m_space;
    TrimeshDataManager * m_trimesh_manager;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AssetFactory::Flags)

#endif // ASSET_FACTORY_H
