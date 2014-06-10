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

/*!
 * \brief Constructs Assets
 */

class AssetFactory : public RenderingObjectsUser
{
public:
    /*!
     * \brief Asset creation flags
     */
    enum Flags_
    {
        SceneNode = 1, ///< If set, asset includes an (invisible) Ogre SceneNode
        Body3D = 2, ///< If set, a visible geometry is attached to the SceneNode
        MeshGeometry = 4, ///< If set, asset will have a geometry taken from SceneNode mesh
        CustomGeometry = 8, ///< If set, user must provide custom function for constructing geometry
        PhyBody = 16 ///< If set, asset will have an ODE body attached
    };
    Q_DECLARE_FLAGS(Flags, Flags_)

    /*!
     * \brief A geometry-constructing function
     */
    typedef std::function<dGeomID (dWorldID, dSpaceID)> GeomFunction;
    /*!
     * \param world physics world to create physics bodies in
     * \param space physics space to use
     * \param trimesh_manager TrimeshDataManager to use
     */
    AssetFactory(dWorldID world, dSpaceID space, TrimeshDataManager * trimesh_manager);


    /*!
     * \param flags asset creation flags
     * \param mesh_name mesh to use for SceneNode or physics geometry
     * \param geometry_func function to construct custom physics geometry
     * \return a newly created asset, owned by caller
     */
    Asset * createAsset(Flags flags, const QString & mesh_name = QString(),
                        GeomFunction geometry_func = nullptr);

private:

    dWorldID m_world;
    dSpaceID m_space;
    TrimeshDataManager * m_trimesh_manager;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AssetFactory::Flags)

#endif // ASSET_FACTORY_H
