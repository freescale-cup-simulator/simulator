#include <physics_simulation.h>

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
    dWorldSetGravity(m_world, 0, 0, GRAVITY_CONSTANT);

    importModel(RESOURCE_DIRECTORY "physics_models/tile.dae", "tile");
    buildTrack();
    createVehicle();
}

PhysicsSimulation::~PhysicsSimulation()
{
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
    dJointSetHinge2Param(m_wheels[2], dParamVel2, data.wheel_power_l);
    dJointSetHinge2Param(m_wheels[2], dParamFMax2, .1);
    dJointSetHinge2Param(m_wheels[3], dParamVel2, data.wheel_power_r);
    dJointSetHinge2Param(m_wheels[3], dParamFMax2, .1);

    const float rad_angle = (data.current_wheel_angle / 180.0) * M_PI;
    for (int i = 0; i < 2; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamLoStop, rad_angle * 3.5);
        dJointSetHinge2Param(m_wheels[i], dParamHiStop, rad_angle * 3.5);
    }

    dSpaceCollide(m_space, this, &PhysicsSimulation::nearCallbackWrapper);
    dWorldStep(m_world, data.physics_timestep);
    dJointGroupEmpty(m_contact_group);

    const dReal * position_v = dBodyGetPosition(m_vehicle_body);
    const dReal * velocity_v = dBodyGetLinearVel(m_vehicle_body);
    const dReal * rotation_q = dBodyGetQuaternion(m_vehicle_body);

    const float velocity = std::sqrt(velocity_v[0]*velocity_v[0]
            + velocity_v[1]*velocity_v[1]
            + velocity_v[2]*velocity_v[2]);


    const float w = rotation_q[0], x = rotation_q[1],
            y = rotation_q[2], z = rotation_q[3];
    float chasis_angle = -std::atan2(2 * (w*z + x*y),
                                      1 - 2 * (y*y + z*z));
#if 1
    qDebug("position %.3f, %.3f, %.3f; rotation %.4f, l.v.m. %.3f",
           position_v[0], position_v[1], position_v[2], chasis_angle, velocity);
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wnarrowing"
#endif
    data.camera_position = {position_v[0], position_v[1], position_v[2] + .65};
    data.camera_rotation = {0.95, 0, chasis_angle};
    QQuaternion q(rotation_q[0],rotation_q[1],rotation_q[2],rotation_q[3]);
    data.camera_rotation_quat=q;

#ifdef __GNUC__
#pragma GCC diagnostic warning "-Wnarrowing"
#endif
}

void PhysicsSimulation::importModel(const QString &path, const QString & name)
{
    const aiScene * s = m_importer.ReadFile(path.toLocal8Bit().data(),
                                            aiProcess_JoinIdenticalVertices |
                                            aiProcess_FixInfacingNormals);
    if (!s)
    {
        qWarning(m_importer.GetErrorString());
        return;
    }
    Q_ASSERT(s->HasMeshes());
    qDebug("Imported %s with %d mesh(es)", path.toLocal8Bit().data(),
           s->mNumMeshes);

    const aiMesh * m = s->mMeshes[0];

    float * vertices = new float[m->mNumVertices * 3];
    for (unsigned int i = 0; i < m->mNumVertices; i++)
    {
        vertices[i * 3]     = m->mVertices[i].x;
        vertices[i * 3 + 1] = m->mVertices[i].y;
        vertices[i * 3 + 2] = m->mVertices[i].z;
    }

    int * indices = new int[m->mNumFaces * 3];
    for (unsigned int i = 0; i < m->mNumFaces; i++)
    {
        indices[i * 3]     = m->mFaces[i].mIndices[0];
        indices[i * 3 + 1] = m->mFaces[i].mIndices[1];
        indices[i * 3 + 2] = m->mFaces[i].mIndices[2];
    }
    dTriMeshDataID id = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle(id, vertices, sizeof(float) * 3, m->mNumVertices,
                                indices, m->mNumFaces * 3, sizeof(int) * 3);
    m_trimesh_data.insert(name, id);
    m_allocated_memory.append({vertices, indices});
    qDebug("Saved dTriMeshDataID = %p", static_cast<void *>(id));
}

void PhysicsSimulation::buildTrack()
{
    for (const tl::Tile & tile : m_track_model.tiles())
    {
        dGeomID id = dCreateTriMesh(m_space, m_trimesh_data["tile"], 0, 0, 0);
        dGeomSetBody(id, 0);
        // shifting top face of tile to z=0 in blender works, don't have to
        // adjust height here
        dGeomSetPosition(id, tile.x() + 0.5, tile.y() + 0.5, 0);
        dMatrix3 rm;
        dRFromEulerAngles(rm, 0, 0, (tile.rotation() / 180.0) * M_PI);
        dGeomSetRotation(id, rm);

        if (tile.type() == tl::Tile::Start)
        {
            m_start_position = dGeomGetPosition(id);
            m_start_direction = dGeomGetRotation(id);
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
    qDebug("Start direction vector: (%.1f,%.1f)", -m_start_direction[4],
            m_start_direction[5]);
#if 0
    // can be useful for debugging
    qDebug("%f %f %f\n%f %f %f\n%f %f %f", m_start_direction[0],
            m_start_direction[1], m_start_direction[2], m_start_direction[4],
            m_start_direction[5], m_start_direction[6], m_start_direction[8],
            m_start_direction[9], m_start_direction[10]);
#endif
}

void PhysicsSimulation::createVehicle()
{
    const dVector3 d = {.16, .25, .01};
    const dReal * sp = m_start_position;
    const dReal spawn_height = .08;
    const dReal radius = .025;

    /*
     * this code is here to support different start directions; we cannot just
     * rotate the vehicle after creating it, so we create it already facing the
     * right direction by rearranging anchor points
     */
    int inv = m_start_direction[5] ? 1 : m_start_direction[5];
    int shift = -m_start_direction[4];
    QVector<QVector<dReal>> anchor_points(4);
    QVector<int> indices;
    const dReal anchor_z = spawn_height - d[2] / 2;

    switch (shift)
    {
    case 1:
        indices = {2, 0, 3, 1};
        break;
    case -1:
        indices = {1, 3, 0, 2};
        break;
    default:
    case 0:
        indices = {0, 1, 2, 3};
        break;
    }

    // front left
    anchor_points[indices[0]] = {sp[0] - d[0] / 2 * inv, sp[1] + d[1] / 2 * inv,
                                 anchor_z};
    // front right
    anchor_points[indices[1]] = {sp[0] + d[0] / 2 * inv, sp[1] + d[1] / 2 * inv,
                                 anchor_z};
    // rear left
    anchor_points[indices[2]] = {sp[0] - d[0] / 2 * inv, sp[1] - d[1] / 2 * inv,
                                 anchor_z};
    // rear right
    anchor_points[indices[3]] = {sp[0] + d[0] / 2 * inv, sp[1] - d[1] / 2 * inv,
                                 anchor_z};

    dBodyID id = dBodyCreate(m_world);
    dGeomID gid = dCreateBox(m_space, d[0], d[1], d[2]);
    dMass mass;
    dJointID jid;

    dBodySetRotation(id, m_start_direction);
    dMassSetBoxTotal(&mass, 1, d[0], d[1], d[2]);
    dBodySetMass(id, &mass);
    dGeomSetBody(gid, id);
    dBodySetPosition(id, sp[0], sp[1], spawn_height);
    m_vehicle_geom = gid;
    m_vehicle_body = id;

    for (int i = 0; i < 4; i++)
    {
        id = dBodyCreate(m_world);
        gid = dCreateSphere(m_space, radius);
        dMassSetSphereTotal(&mass, 0.1, radius);
        dBodySetMass(id, &mass);
        dBodySetPosition(id, anchor_points[i][0], anchor_points[i][1],
                anchor_points[i][2]);
        dGeomSetBody(gid, id);

        jid = dJointCreateHinge2(m_world, 0);
        dJointAttach(jid, m_vehicle_body, id);
        dJointSetHinge2Anchor(jid, anchor_points[i][0], anchor_points[i][1],
                anchor_points[i][2]);
        dJointSetHinge2Axis1(jid, 0, 0, 1);
        dJointSetHinge2Axis2(jid, m_start_direction[5], m_start_direction[4], 0);
        dJointSetHinge2Param(jid, dParamSuspensionERP, 0.4);
        dJointSetHinge2Param(jid, dParamSuspensionCFM, 0.1);
        m_wheels[i] = jid;
        m_wheel_bodies[i] = id;
    }
    for (int i = 2; i < 4; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamLoStop, 0);
        dJointSetHinge2Param(m_wheels[i], dParamHiStop, 0);
    }
}

void PhysicsSimulation::nearCallback(void *, dGeomID ga, dGeomID gb)
{
    // we only want vehicle-track collisions
    bool is_ground_collision = m_track_geoms.contains(ga)
            || m_track_geoms.contains(gb);
    if (!is_ground_collision)
        return;

    dContact contacts[MAX_CONTACTS];

    for (int i = 0; i < MAX_CONTACTS; i++)
    {
        contacts[i].surface.mode = dContactSoftERP | dContactSoftCFM
                | dContactSlip1 | dContactSlip2;
        contacts[i].surface.mu = dInfinity;
        contacts[i].surface.soft_erp = 0.5;
        contacts[i].surface.soft_cfm = .001;
        contacts[i].surface.slip1 = .1;
        contacts[i].surface.slip2 = .1;
    }

    int nc = dCollide(ga, gb, MAX_CONTACTS, &contacts[0].geom, sizeof(dContact));
    if (nc)
    {
        for (int i = 0; i < nc; i++)
        {
            dJointID c = dJointCreateContact (m_world, m_contact_group,
                                              &contacts[i]);
            dJointAttach (c, dGeomGetBody(ga), dGeomGetBody(gb));
        }
    }
}

void PhysicsSimulation::nearCallbackWrapper(void * i, dGeomID a, dGeomID b)
{
    static_cast<PhysicsSimulation *>(i)->nearCallback(nullptr, a, b);
}
