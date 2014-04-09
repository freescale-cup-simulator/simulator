#include <asset_manager.h>

TrimeshDataManager::TrimeshDataManager(QQuickWindow *surface,QOpenGLContext * context, Ogre::SceneManager *scene_manager)
    : m_mesh_man(0)
    , m_scene_manager(scene_manager)
    , m_surface(surface)
    , m_gl_context(context)
{
    m_mesh_man=Ogre::MeshManager::getSingletonPtr();
}

TrimeshDataManager::~TrimeshDataManager()
{
    foreach (dTriMeshDataID tid, m_container.values()) {
        dGeomTriMeshDataDestroy (tid);
    }
    for (auto pair : m_allocated_memory)
    {
        delete pair.first;
        delete pair.second;
    }
}

const dTriMeshDataID & TrimeshDataManager::value(const QString &key)
{
    if (!m_container.contains(key))
       return append(key);
    else
        return m_container[key];
}

const dTriMeshDataID &TrimeshDataManager::operator[](const QString &key)
{
    return m_container[key];
}

// mesh loading from QML THREAD!!!!!

dTriMeshDataID & TrimeshDataManager::append(const QString &key)
{
    dTriMeshDataID tid;
    if (!m_mesh_man)
        qFatal("Null reference to ogre mesh manager.");
    glPopAttrib();
    glPopClientAttrib();
    m_gl_context->makeCurrent(m_surface);
    /*auto it=m_mesh_man;
    while(it.hasMoreElements())
    {
        qDebug()<<QString::fromStdString(it.getNext()->getName());
    }*/
    //Ogre::Entity* ent=m_scene_manager->createEntity(key.toStdString().c_str());

    Ogre::MeshPtr mesh=m_mesh_man->load(key.toLocal8Bit().data(),
                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //Ogre::MeshPtr mesh = ent->getMesh();//m_mesh_man->getByName(key.toLocal8Bit().data());
    if (mesh.isNull())
        qFatal("Cannot find requested mesh for physics simulation!");

    size_t vertex_count, index_count;
    float * vertices;
    unsigned int * indices;
    getOgreMeshData(mesh.get(), vertex_count, vertices, index_count, indices);
    tid = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle(tid, vertices, sizeof(float) * 3, vertex_count,
                                indices, index_count, sizeof(int) * 3);
    m_allocated_memory.append({vertices, indices});
    m_gl_context->doneCurrent();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    return m_container.insert(key,tid).value();
}

void TrimeshDataManager::getOgreMeshData(const Ogre::Mesh * const mesh, size_t &vertex_count, float *&vertices, size_t &index_count, unsigned int *&indices)
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
            if (submesh->useSharedVertices)
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


AssetFactory::AssetFactory(dWorldID world,
                            dSpaceID space,
                            TrimeshDataManager *trimesh_manager,
                            Ogre::SceneManager *scene_manager)
    : m_world(world)
    , m_space(space)
    , m_trimesh_manager(trimesh_manager)
    , m_scene_manager(scene_manager)
    , m_gl_context(m_trimesh_manager->context())
    , m_surface(m_trimesh_manager->surface())
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
        m_gl_context->makeCurrent(m_surface);
        if(flags & SceneNode)
        {
            node3d=m_scene_manager->getRootSceneNode()->createChildSceneNode();
            node3d->setVisible(false,true);
        }
        if((flags & SceneNode) && (flags & Body3D))
        {
            Ogre::Entity* ent=m_scene_manager->createEntity(mesh_name.toStdString().c_str());
            node3d->attachObject(ent);
        }
        m_gl_context->doneCurrent();
    }
    return new Asset(node3d,bid,gid);
}


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
