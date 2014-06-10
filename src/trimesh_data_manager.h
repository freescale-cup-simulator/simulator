#ifndef TRIMESH_DATA_MANAGER_H
#define TRIMESH_DATA_MANAGER_H

#include <ode/ode.h>
#include <OgreRoot.h>
#include <QString>

#include <global_renderer.h>

/*!
 * \brief Manages ODE TriMeshData to prevent duplication and unnecessary re-loading
 *
 * When a dTriMeshDataID is requested via value() or operator[], it is either created
 * from an Ogre Mesh or stored, or a previously stored dTriMeshDataID is returned.
 * This avoids duplication of dTriMeshDataIDs in case when there are multiple objects
 * with the same geometry/mesh (track tiles, for example)
 */
class TrimeshDataManager : public RenderingObjectsUser
{
public:
    TrimeshDataManager();
    ~TrimeshDataManager();

    const dTriMeshDataID & value(const QString & key);
    const dTriMeshDataID & operator[](const QString & key);

    inline void setRenderingObjects(RenderingObjects * ro)
    {
        RenderingObjectsUser::setRenderingObjects(ro);
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
};

#endif // TRIMESH_DATA_MANAGER_H
