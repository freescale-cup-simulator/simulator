#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include <ode/ode.h>
#include <OgreRoot.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreSubMesh.h>
#include <OGRE/OgreVertexIndexData.h>
#include <OGRE/OgreMeshManager.h>

#include <QObject>
#include <QString>
#include <QHash>

struct WorldObject
{
    dTriMeshDataID trimesh_data;
    Ogre::Entity * entity;
};

class MeshManager : public QObject
{
public:
    MeshManager(QObject * parent);
    ~MeshManager();
    bool importModel(const QString & key, const QString & mesh_name,bool create_ogre_entity=true);
private:

    void getOgreMeshData(const Ogre::Mesh * const mesh,
                                            size_t & vertex_count,
                                            float * & vertices, size_t & index_count,
                                            unsigned int * & indices);

    QVector<QPair<float *, unsigned int *>> m_allocated_memory;
    QHash<QString, WorldObject> m_world_objects;
};

#endif
