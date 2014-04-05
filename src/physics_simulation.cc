#include <physics_simulation.h>

// utility functions
namespace
{

inline QVector3D dv2qv (const dReal * dv)
{
    return QVector3D (dv[0], dv[1], dv[2]);
}

inline QQuaternion dq2qq(const dReal * q)
{
    return QQuaternion (q[0], q[1], q[2], q[3]);
}

inline qreal anglev (const QVector3D & a, const QVector3D & b)
{
    const qreal v = QVector3D::dotProduct(a, b) / (a.length() * b.length());
    return (std::acos(v) / M_PI) * 180.0;
}

}

PhysicsSimulation::PhysicsSimulation(const track_library::TrackModel &model,
                                     QObject * parent)
    : QObject(parent)
    , m_track_model(model)
    , m_start_position(nullptr)
{
    dInitODE();
    m_world = dWorldCreate();
    m_space = dSimpleSpaceCreate(nullptr);
    m_contact_group = dJointGroupCreate(0);
    dWorldSetGravity(m_world, 0, GRAVITY_CONSTANT, 0);

    importModel("line.mesh", "tile");
    importModel("car_body.mesh", "car");
    buildTrack();
    createVehicle();
}

PhysicsSimulation::~PhysicsSimulation()
{
    qDebug()<<"Physics destructor";
    dJointGroupDestroy(m_contact_group);
    dSpaceDestroy(m_space);
    dWorldDestroy(m_world);
    dCloseODE();
    for (auto pair : m_allocated_memory)
    {
        delete pair.first;
        delete pair.second;
    }
}

void PhysicsSimulation::process(DataSet & data)
{
    updateERPandCFM(data);

    for (int i = 2; i < 4; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamVel2, getPropertyModelInstance()->getPropertyValue("vel2"));
        dJointSetHinge2Param(m_wheels[i], dParamFMax2, getPropertyModelInstance()->getPropertyValue("fmax"));
    }

    const float rad_angle = (data.current_wheel_angle / 180.0) * M_PI;
    for (int i = 0; i < 2; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamLoStop1, rad_angle * 1.0 - 1e-3);
        dJointSetHinge2Param(m_wheels[i], dParamHiStop1, rad_angle * 1.0 + 1e-3);
    }

    for (int i = 0; i < 4; i++)
    {
        dBodySetFiniteRotationAxis(m_wheel_bodies[i], 0, 0, 0);
    }

    dSpaceCollide(m_space, this, &PhysicsSimulation::nearCallbackWrapper);

    for (int i = 0; i < m_track_geoms.count(); i++)
        setTrimeshLastTransform(m_track_geoms[i]);
    setTrimeshLastTransform(m_vehicle_geom);

    dWorldStep(m_world, data.physics_timestep);
    dJointGroupEmpty(m_contact_group);

    updateBodyData(data);

    m_vehicleVelocity = dv2qv(dBodyGetLinearVel(m_vehicle_body)).length();
    vehicleVelocityChanged();
}

qreal PhysicsSimulation::vehicleVelocity()
{
    return m_vehicleVelocity;
}

void PhysicsSimulation::importModel(const QString & mesh_name,
                                    const QString & name)
{
    Ogre::MeshManager * mm = Ogre::MeshManager::getSingletonPtr();
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
    m_trimesh_data.insert(name, id);
    m_allocated_memory.append({vertices, indices});
}

void PhysicsSimulation::buildTrack()
{
    int rotation;

    for (const tl::Tile & tile : m_track_model.tiles())
    {
        dGeomID id = dCreateTriMesh(m_space, m_trimesh_data["tile"], 0, 0, 0);
        dQuaternion rotation_q;

        // make the geom static
        dGeomSetBody(id, 0);
        dGeomSetPosition(id, -tile.x() + 0.5, 0, tile.y() + 0.5);
        dQFromAxisAndAngle(rotation_q, 0, 1, 0, (-tile.rotation() / 180.0) * M_PI);
        dGeomSetQuaternion(id, rotation_q);

        if (tile.type() == tl::Tile::Start)
        {
            m_start_position = dGeomGetPosition(id);
            dGeomGetQuaternion(id, m_start_rotation_q);
            rotation = tile.rotation();
        }
        m_track_geoms.append(id);
    }

    if (m_start_position == nullptr)
        qFatal("Start tile not present, simulation will break");

    /*
     * NOTE: layout of dMatrix3:
     * ux uy uz . <- x vector
     * vx vy vz . <- y vector
     * wx wy wz . <- z vector
     * ( . is ignored, because dMatrix4 is of same size)
     */

    qDebug("Start location: (%.1f,%.1f,%.1f)", m_start_position[0],
            m_start_position[1], m_start_position[2]);

    QQuaternion q = QQuaternion::fromAxisAndAngle(0, 1, 0, rotation);
    m_start_rotation_v = q.rotatedVector({1, 0, 0});

    qDebug() << "Start direction: " << m_start_rotation_v;
}

void PhysicsSimulation::createVehicle()
{
    constexpr dReal spawn_height = 0.1;
    constexpr dReal mass = 2;

    dBodyID id = dBodyCreate(m_world);
    dGeomID gid = dCreateTriMesh(m_space, m_trimesh_data["car"], 0, 0, 0);
    dJointID jid;
    dMass m;
    dVector3 sp = {m_start_position[0], m_start_position[1] + spawn_height,
                   m_start_position[2], 0};

    dMassSetZero(&m);
    dMassSetTrimeshTotal(&m, mass, gid);
    dGeomSetPosition(gid, -m.c[0], -m.c[1], -m.c[2]);
    dMassTranslate(&m, -m.c[0], -m.c[1], -m.c[2]);
    dGeomSetBody(gid, id);
    dBodySetMass(id, &m);
    dBodySetPosition(id, sp[0], sp[1], sp[2]);
    dBodySetQuaternion(id, m_start_rotation_q);

    m_vehicle_geom = gid;
    m_vehicle_body = id;

    Ogre::MeshManager * mm = Ogre::MeshManager::getSingletonPtr();
    Ogre::MeshPtr mesh = mm->getByName("car_body.mesh");
    Ogre::Quaternion start_rotation_ (m_start_rotation_q[0], m_start_rotation_q[1],
            m_start_rotation_q[2], m_start_rotation_q[3]);
    auto skeleton = mesh->getSkeleton();

    auto cp = skeleton->getBone("cam")->_getDerivedPosition();
    cp = start_rotation_ * cp;
    jid = dJointCreateFixed(m_world, 0);
    m_camera_body = dBodyCreate(m_world);

    cp.x += sp[0];
    cp.y += sp[1];
    cp.z += sp[2];
    dBodySetPosition(m_camera_body, cp.x, cp.y, cp.z);
    dBodySetQuaternion(m_camera_body, m_start_rotation_q);
    dJointAttach(jid, m_vehicle_body, m_camera_body);
    dJointSetFixed(jid);

    constexpr int nwheels = 4;
    constexpr float radius = 0.025;
    const char * wn[nwheels] = {"wheel_hl", "wheel_hr", "wheel_rl", "wheel_rr"};

    for (int i = 0; i < nwheels; i++)
    {
        auto v = skeleton->getBone(wn[i])->_getDerivedPosition();
        v = start_rotation_ * v;
        v.x += sp[0];
        v.y += sp[1];
        v.z += sp[2];

        id = dBodyCreate(m_world);
        gid = dCreateSphere(m_space, radius);
        dMassSetSphereTotal(&m, 0.5, radius);
        dBodySetMass(id, &m);
        dBodySetPosition(id, v[0], v[1], v[2]);
        dBodySetQuaternion(id, m_start_rotation_q);
        dGeomSetBody(gid, id);
        dBodySetFiniteRotationMode(id, 1);

        jid = dJointCreateHinge2(m_world, 0);
        dJointAttach(jid, m_vehicle_body, id);
        dJointSetHinge2Anchor(jid, v[0], v[1], v[2]);
        dJointSetHinge2Axis1(jid, 0, 1, 0);
        dJointSetHinge2Axis2(jid, -m_start_rotation_v.x(), 0,
                             -m_start_rotation_v.z());

        m_wheels[i] = jid;
        m_wheel_bodies.append(id);
    }

    for (int i = 2; i < 4; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamLoStop, -1e-3);
        dJointSetHinge2Param(m_wheels[i], dParamHiStop, 1e-3);
    }
}

void PhysicsSimulation::nearCallback(void *, dGeomID ga, dGeomID gb)
{
    // we only want vehicle-track collisions
    if (!(m_track_geoms.contains(ga) ^ m_track_geoms.contains(gb)))
        return;

    dBodyID ba = dGeomGetBody(ga), bb = dGeomGetBody(gb);
    dContact contacts[MAX_CONTACTS];
    QQuaternion body_q (dq2qq(dBodyGetQuaternion(m_vehicle_body)));

    {
        int wheel_index = m_wheel_bodies.contains(ba) ? m_wheel_bodies.indexOf(ba)
                                                      : m_wheel_bodies.indexOf(bb);
        if (wheel_index <= 1 && wheel_index >= 0)
        {
            dReal a = dJointGetHinge2Param(m_wheels[wheel_index], dParamLoStop1);
            body_q *= QQuaternion::fromAxisAndAngle(0, 1, 0, (-a / M_PI) * 180.0);
        }
    }

    QVector3D fdir1_v = body_q.rotatedVector( { -m_start_rotation_v.z(),
                                                0,
                                                m_start_rotation_v.x() });
    fdir1_v.normalize();

    auto s       = getPropertyModelInstance();
    double slip1 = s->getPropertyValue("slip1");
    double slip2 = s->getPropertyValue("slip2");
    double mu    = s->getPropertyValue("mu");
    double rho   = s->getPropertyValue("rho");

    for (int i = 0; i < MAX_CONTACTS; i++)
    {
        contacts[i].surface.mode = dContactSoftCFM | dContactSoftERP
                | dContactSlip1 | dContactSlip2 | dContactFDir1
                | dContactApprox1 | dContactRolling;
        contacts[i].surface.mu = mu;
        contacts[i].surface.soft_cfm = m_surfaceCFM;
        contacts[i].surface.soft_erp = m_surfaceERP;
        contacts[i].surface.rho = rho;
        contacts[i].surface.rho2 = rho;
        contacts[i].surface.rhoN = rho;
        contacts[i].fdir1[0] = fdir1_v.x();
        contacts[i].fdir1[1] = 0;
        contacts[i].fdir1[2] = fdir1_v.z();
        contacts[i].surface.slip1 = slip1 * m_vehicleVelocity;
        contacts[i].surface.slip2 = slip2 * m_vehicleVelocity;
    }

    int nc = dCollide(ga, gb, MAX_CONTACTS, &contacts[0].geom, sizeof(dContact));
    for (int i = 0; i < nc; i++)
    {
        dJointID c = dJointCreateContact(m_world, m_contact_group, &contacts[i]);
        dJointAttach(c, dGeomGetBody(ga), dGeomGetBody(gb));
    }
}

void PhysicsSimulation::nearCallbackWrapper(void * i, dGeomID a, dGeomID b)
{
    static_cast<PhysicsSimulation *>(i)->nearCallback(nullptr, a, b);
}

// originally found on Ogre Wiki
void PhysicsSimulation::getOgreMeshData(const Ogre::Mesh * const mesh,
                                        size_t & vertex_count,
                                        float * & vertices, size_t & index_count,
                                        unsigned int * & indices)
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

void PhysicsSimulation::updateBodyData(DataSet & d)
{
    const dReal * p;
    const dReal * q;

/*
 * if ODE is built with double precision, disable "narrowing" warnings because
 * QVector3D and QQuaternion use floats
 */
#if defined(__GNUC__) && defined(dDOUBLE)
#pragma GCC diagnostic ignored "-Wnarrowing"
#endif

    p = dBodyGetPosition(m_camera_body);
    q = dBodyGetQuaternion(m_camera_body);
    d.camera.p = { p[0], p[1], p[2] };
    d.camera.q = { q[0], q[1], q[2], q[3] };

    p = dBodyGetPosition(m_vehicle_body);
    q = dBodyGetQuaternion(m_vehicle_body);
    d.vehicle.p = { p[0], p[1], p[2] };
    d.vehicle.q = { q[0], q[1], q[2], q[3] };

    for (int i = 0; i < 4; i++)
    {
        p = dBodyGetPosition(m_wheel_bodies[i]);
        q = dBodyGetQuaternion(m_wheel_bodies[i]);

        d.wheels[i].p = { p[0], p[1], p[2] };
        d.wheels[i].q = { q[0], q[1], q[2], q[3] };
    }

#if defined(__GNUC__) && defined(dDOUBLE)
#pragma GCC diagnostic warning "-Wnarrowing"
#endif
}

void PhysicsSimulation::updateERPandCFM(const DataSet &d)
{
    auto s = getPropertyModelInstance();

    const double spring_k = s->getPropertyValue("spring_k");
    const double spring_damping = s->getPropertyValue("spring_damping");

    const double tire_k = s->getPropertyValue("tire_k");
    const double tire_damping = s->getPropertyValue("tire_damping");

    const double dt = d.physics_timestep;

    m_surfaceERP = dt * spring_k / (dt * spring_k + spring_damping);
    m_surfaceCFM = 1.0 / (dt * spring_k + spring_damping);

    dReal wERP = dt * tire_k / (dt * tire_k + tire_damping);
    dReal wCFM = 1.0 / (dt * tire_k + tire_damping);

    for (int i = 0; i < 4; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamSuspensionERP, wERP);
        dJointSetHinge2Param(m_wheels[i], dParamSuspensionCFM, wCFM);
    }
}
