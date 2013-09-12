#include <physics_simulation.h>

namespace {

// compute angle between two vectors
inline float va (dReal ax, dReal ay, dReal az,
                 dReal bx, dReal by, dReal bz)
{
    float dot = ax*bx + ay*by + az*bz;
    float ma = std::sqrt(ax*ax + ay*ay + az*az);
    float mb = std::sqrt(bx*bx + by*by + bz*bz);
    float c = dot / (ma * mb);
    return (std::acos(c) / M_PI) * 180;
}

}

PhysicsSimulation::PhysicsSimulation(const track_library::TrackModel &model)
    : m_track_model(model)
    , m_frame_duration(0.01)
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
    for (auto p : m_allocated_memory)
    {
        delete p.first;
        delete p.second;
    }
}

dReal
PhysicsSimulation::getFrameDuration()
{
    return m_frame_duration;
}

void
PhysicsSimulation::setFrameDuration(dReal duration)
{
    m_frame_duration = duration;
}

DataSet
PhysicsSimulation::onModelResponse(const DataSet & control)
{
    // stepping with smaller step yields more precision, and yet
    // our control algorithm will have to run with longer intervals;
    // so we do multiple steps to achieve desired interval
    for (int i = 0; i < m_frame_duration / WORLD_STEP; i++)
    {
        dSpaceCollide(m_space, this, &PhysicsSimulation::nearCallbackWrapper);
        dWorldQuickStep(m_world, WORLD_STEP);
        dJointGroupEmpty(m_contact_group);
        for (int i = 2; i < 4; i++)
        {
            dJointSetHinge2Param(m_wheels[i], dParamVel2, 40);
            dJointSetHinge2Param(m_wheels[i], dParamFMax2, 16e1);
        }

        for (int i = 0; i < 2; i++)
        {
            dReal a = dJointGetHinge2Angle1(m_wheels[i]);
//            qDebug("%.2f %.2f", a, control[MovementAngle].toFloat());
            a = -a;
//            a = control[MovementAngle].toFloat() - a;
//            if (a > .1) a = .1;
//            if (a < -.1) a = -.1;
            a *= 10.0;
            dJointSetHinge2Param(m_wheels[i], dParamVel, a);
            dJointSetHinge2Param(m_wheels[i], dParamFMax, 16e1);
            dJointSetHinge2Param(m_wheels[i], dParamLoStop, -1);
            dJointSetHinge2Param(m_wheels[i], dParamHiStop, 1);
            dJointSetHinge2Param(m_wheels[i], dParamFudgeFactor, 0.01);
        }
    }

    const dReal * p = dBodyGetPosition(m_vehicle_body);
    const dReal * r = dBodyGetRotation(m_vehicle_body);
    const dReal * v = dBodyGetLinearVel(m_vehicle_body);
    const float vel = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    const float a = va(0, 1, 0, r[4], r[5], r[6]);
//    qDebug("%.2f %.2f %.2f", r[4], r[5], r[6]);
    qDebug("position %.3f, %.3f, %.3f; rotation %.2f, linear velocity mag. %.3f",
           p[0], p[1], p[2], a, vel);

    DataSet s;

    s[CameraPositionX] = p[0] + r[4] * .3;
    s[CameraPositionY] = p[1] + r[5] * .3;
    s[CameraPositionZ] = p[2] + .75;
    s[CameraRotationX] = 60;
    s[CameraRotationY] = a;
    s[CameraRotationZ] = 0;

//    qDebug("camera %.3f, %.3f", s[CameraPositionX].toFloat(),
//           s[CameraPositionY].toFloat());

    return s;
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

void
PhysicsSimulation::buildTrack()
{
    for (const tl::Tile & tile : m_track_model.tiles())
    {
        dGeomID id = dCreateTriMesh(m_space, m_trimesh_data["tile"], 0, 0, 0);
        dGeomSetBody(id, 0);
        // shifting top face of tile to z=0 in blender works, don't have to
        // adjust height here
        dGeomSetPosition(id, tile.x() + 0.5, tile.y() + 0.5, 0);

        dMatrix3 rm;
        dRFromEulerAngles(rm, 0, 0, (tile.rotation() / 180) * M_PI);
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
    qDebug("Start direction vector: (%.1f,%.1f,%.1f)", m_start_direction[4],
            m_start_direction[5], m_start_direction[6]);
}

void PhysicsSimulation::createVehicle()
{
    const dVector3 d = {.16, .25, .1};
    const dReal * sp = m_start_position;
    const dReal spawn_height = .08;
    const dReal radius = .02;
    const dReal a[4][3] =
    {
        { sp[0] - d[0] / 2, sp[1] + d[1] / 2, spawn_height - d[2] / 2 },
        { sp[0] + d[0] / 2, sp[1] + d[1] / 2, spawn_height - d[2] / 2 },
        { sp[0] - d[0] / 2, sp[1] - d[1] / 2, spawn_height - d[2] / 2 },
        { sp[0] + d[0] / 2, sp[1] - d[1] / 2, spawn_height - d[2] / 2 },
    };

    dBodyID id = dBodyCreate(m_world);
    dGeomID gid = dCreateBox(m_space, d[0], d[1], d[2]);
    dMass mass;
    dJointID jid;

    dMassSetBoxTotal(&mass, 1.27, d[0], d[1], d[2]);
    dBodySetMass(id, &mass);
    dGeomSetBody(gid, id);
    dBodySetPosition(id, sp[0], sp[1], spawn_height);
    m_vehicle_geom = gid;
    m_vehicle_body = id;

    for (int i = 0; i < 4; i++)
    {
        id = dBodyCreate(m_world);
        gid = dCreateCylinder(m_space, radius, radius * 2);
        dMassSetCylinderTotal(&mass, 0.2, 1, radius, radius * 2);
        dBodySetMass(id, &mass);
        dBodySetPosition(id, a[i][0], a[i][1], a[i][2]);
        dGeomSetBody(gid, id);
        dMatrix3 r;
        dRFromEulerAngles(r, 0, M_PI_2, 0);
        dBodySetRotation(id, r);

        jid = dJointCreateHinge2(m_world, 0);
        dJointAttach(jid, m_vehicle_body, id);
        dJointSetHinge2Anchor(jid, a[i][0], a[i][1], a[i][2]);
        dJointSetHinge2Axis1(jid, 0, 0, 1);
        dJointSetHinge2Axis2(jid, 1, 0, 0);
        dJointSetHinge2Param(jid, dParamSuspensionERP, 0.6);
        dJointSetHinge2Param(jid, dParamSuspensionCFM, 0.2);
        m_wheels[i] = jid;
    }

    dJointSetHinge2Param(m_wheels[2], dParamLoStop, 0);
    dJointSetHinge2Param(m_wheels[3], dParamHiStop, 0);
}

void PhysicsSimulation::nearCallback(void *, dGeomID ga, dGeomID gb)
{
    // we only want vehicle-track collisions
    bool is_ground_collision = m_track_geoms.contains(ga)
            || m_track_geoms.contains(gb);
    if (!is_ground_collision)
        return;

    dBodyID a = dGeomGetBody(ga);
    dBodyID b = dGeomGetBody(gb);
    dContact contacts[MAX_CONTACTS];

    for (int i = 0; i < MAX_CONTACTS; i++)
    {
        contacts[i].surface.mode = dContactSoftERP | dContactSoftCFM
                 | dContactSlip1 | dContactSlip2;
        contacts[i].surface.mu = dInfinity;
        contacts[i].surface.soft_erp = 0.6;
        contacts[i].surface.soft_cfm = 1e-3;
        contacts[i].surface.slip1 = .01;
        contacts[i].surface.slip2 = .01;
    }

    int nc = dCollide(ga, gb, MAX_CONTACTS, &contacts[0].geom, sizeof(dContact));
    if (nc)
    {
        for (int i = 0; i < nc; i++)
        {
            dJointID c = dJointCreateContact (m_world, m_contact_group,
                                              &contacts[i]);
            dJointAttach (c, a, b);
        }
    }
}

void PhysicsSimulation::nearCallbackWrapper(void * i, dGeomID a, dGeomID b)
{
    static_cast<PhysicsSimulation *>(i)->nearCallback(nullptr, a, b);
}
