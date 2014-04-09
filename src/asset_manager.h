#ifndef MESHMANAGER_H
#define MESHMANAGER_H

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

class TrimeshDataManager
{

public:
    TrimeshDataManager();
    ~TrimeshDataManager();

    const dTriMeshDataID & value(const QString & key);
    const dTriMeshDataID & operator[](const QString & key);

    inline void setRenderingInstances(GlobalRenderer::RenderingInstances * i)
    {
        m_rendering_instances = i;
        // get MeshManager at this point because this method is called after
        // rendering has been initialised
        m_mesh_man = Ogre::MeshManager::getSingletonPtr();
    }

private:
    dTriMeshDataID & append(const QString & key);
    void getOgreMeshData(const Ogre::Mesh * const mesh,
                                            size_t & vertex_count,
                                            float * & vertices, size_t & index_count,
                                            unsigned int * & indices);

    QVector<QPair<float *, unsigned int *>> m_allocated_memory;
    Ogre::MeshManager * m_mesh_man;
    QHash<QString,dTriMeshDataID>  m_container;

    GlobalRenderer::RenderingInstances * m_rendering_instances;
};

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


#endif
