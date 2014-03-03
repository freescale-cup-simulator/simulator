#include <mesh_manager.h>

MeshManager::MeshManager(QObject *parent)
{
    dInitODE();
}

MeshManager::~MeshManager()
{
    foreach (WorldObject obj, m_world_objects.values()) {
        dGeomTriMeshDataDestroy (obj.trimesh_data);
    }
    for (auto pair : m_allocated_memory)
    {
        delete pair.first;
        delete pair.second;
    }
    dCloseODE();

}

bool MeshManager::importModel(const QString &key, const QString &mesh_name, bool create_ogre_entity)
{
    Ogre::MeshManager * mm = Ogre::MeshManager::getSingletonPtr();

    if (!mm) return false;

    Ogre::MeshPtr mesh = mm->getByName(mesh_name.toLocal8Bit().data());

    if (mesh.isNull())
        qFatal("Cannot find requested mesh for physics simulation!");

    size_t vertex_count, index_count;
    float * vertices;
    unsigned int * indices;
    getOgreMeshData(mesh.get(), vertex_count, vertices, index_count, indices);

    dTriMeshDataID id = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle(id, vertices, sizeof(float) * 3, vertex_count,
                                indices, index_count, sizeof(int) * 3);
    m_allocated_memory.append({vertices, indices});
    WorldObject obj;
    obj.trimesh_data=id;

    if (create_ogre_entity)
        obj.entity=Ogre::Root::getSingleton().getSceneManager("SceneManager")->createEntity(mesh_name);
    else
        obj.entity=0;
    m_world_objects.insert(key,obj);
    return true;
}

void MeshManager::getOgreMeshData(const Ogre::Mesh * const mesh, size_t &vertex_count, float *&vertices, size_t &index_count, unsigned int *&indices)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }
        // Add the indices
        index_count += submesh->indexData->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new float[vertex_count * 3];
    indices = new unsigned int[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                    vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                    vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                    static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; j++, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                vertices[current_offset + j*3]     = pReal[0];
                vertices[current_offset + j*3 + 1] = pReal[1];
                vertices[current_offset + j*3 + 2] = pReal[2];
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<unsigned int>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned int>(pShort[k]) +
                        static_cast<unsigned int>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}
