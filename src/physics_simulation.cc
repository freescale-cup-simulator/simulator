#include <physics_simulation.h>

PhysicsSimulation::PhysicsSimulation(QObject * parent)
    : QObject(parent)
    , m_world(0)
    , m_space(0)
    , m_contact_group(0)
{
    dInitODE();

    /*importModel("line.mesh", "tile");
    importModel("car_body.mesh", "car");
    buildTrack();
    createVehicle();*/
}

PhysicsSimulation::~PhysicsSimulation()
{
    qDebug()<<"Physics destructor";
    if(m_world)
    {
        dJointGroupDestroy(m_contact_group);
        dSpaceDestroy(m_space);
        dWorldDestroy(m_world);
    }
    dCloseODE();
}

void PhysicsSimulation::createWorld()
{
    if(m_world)
    {
        dJointGroupDestroy(m_contact_group);
        dSpaceDestroy(m_space);
        dWorldDestroy(m_world);
        m_world=0;
        m_space=0;
        m_contact_group=0;
    }
    m_world = dWorldCreate();
    m_space = dSimpleSpaceCreate(nullptr);
    m_contact_group = dJointGroupCreate(0);
    dWorldSetGravity(m_world, 0, GRAVITY_CONSTANT, 0);
}

void PhysicsSimulation::process(DataSet & data)
{
    for (int i = 2; i < 4; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamVel2, 25);
        dJointSetHinge2Param(m_wheels[i], dParamFMax2, .002);
    }

    const float rad_angle = (data.current_wheel_angle / 180.0) * M_PI;
    for (int i = 0; i < 2; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamLoStop1, rad_angle * 1.0 - 1e-3);
        dJointSetHinge2Param(m_wheels[i], dParamHiStop1, rad_angle * 1.0 + 1e-3);
    }

    dSpaceCollide(m_space, this, &PhysicsSimulation::nearCallbackWrapper);

    for (int i = 0; i < m_track_geoms.count(); i++)
        setTrimeshLastTransform(m_track_geoms[i]);
    setTrimeshLastTransform(m_vehicle_geom);

    dWorldStep(m_world, data.physics_timestep);
    dJointGroupEmpty(m_contact_group);

    updateBodyData(data);

#if 0
    const dReal * q = dBodyGetQuaternion(m_wheel_bodies[3]);
    const float a = std::asin(2 * (q[0]*q[2] - q[3]*q[1]));
    qDebug() << "wheel angle: " << a;

    const dReal * velocity_v = dBodyGetLinearVel(m_vehicle_body);
    const float velocity = std::sqrt(velocity_v[0]*velocity_v[0]
            + velocity_v[1]*velocity_v[1]
            + velocity_v[2]*velocity_v[2]);

    qDebug("position %.3f, %.3f, %.3f; rotation %.4f, l.v.m. %.3f",
           position_v[0], position_v[1], position_v[2], chasis_angle, velocity);
#endif
}

void PhysicsSimulation::buildTrack()
{
    int rotation;

    /*for (const tl::Tile & tile : m_track_model.tiles())
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

    const char * d[] = {"Up", "Right", "Down", "Left"};

    switch(rotation)
    {
    case 0:
        m_start_direction = Up;
        break;
    case 90:
        m_start_direction = Right;
        break;
    case 180:
        m_start_direction = Down;
        break;
    case 270:
        m_start_direction = Left;
        break;
    }

    qDebug("Start direction: %s", d[m_start_direction]);
}

void PhysicsSimulation::createVehicle()
{
    constexpr dReal spawn_height = 0.1;
    constexpr dReal mass = 1.27;

    dBodyID id = dBodyCreate(m_world);
    /*dGeomID gid = dCreateTriMesh(m_space, m_trimesh_data["car"], 0, 0, 0);
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
        dMassSetSphereTotal(&m, 0.3, radius);
        dBodySetMass(id, &m);
        dBodySetPosition(id, v[0], v[1], v[2]);
        dBodySetQuaternion(id, m_start_rotation_q);
        dGeomSetBody(gid, id);

        jid = dJointCreateHinge2(m_world, 0);
        dJointAttach(jid, m_vehicle_body, id);
        dJointSetHinge2Anchor(jid, v[0], v[1], v[2]);
        dJointSetHinge2Axis1(jid, 0, 1, 0);

        switch(m_start_direction)
        {
        case Up:
            dJointSetHinge2Axis2(jid, -1, 0, 0);
            break;
        case Down:
            dJointSetHinge2Axis2(jid, 1, 0, 0);
            break;
        case Left:
            dJointSetHinge2Axis2(jid, 0, 0, 1);
            break;
        case Right:
            dJointSetHinge2Axis2(jid, 0, 0, -1);
            break;
        }

        dJointSetHinge2Param(jid, dParamSuspensionERP, 0.35);
        dJointSetHinge2Param(jid, dParamSuspensionCFM, 0.025);
        m_wheels[i] = jid;
        m_wheel_bodies.append(id);
    }

    for (int i = 2; i < 4; i++)
    {
        dJointSetHinge2Param(m_wheels[i], dParamLoStop, -1e-3);
        dJointSetHinge2Param(m_wheels[i], dParamHiStop, 1e-3);
    }*/
}

void PhysicsSimulation::nearCallback(void *, dGeomID ga, dGeomID gb)
{
    // we only want vehicle-track collisions
    const bool cga = m_track_geoms.contains(ga);
    const bool cgb = m_track_geoms.contains(gb);
    const bool ground_collision = (cga && !cgb) || (!cga && cgb);
    if (!ground_collision)
        return;

    dContact contacts[MAX_CONTACTS];
    int wheel_index;

    const dReal * q = dBodyGetQuaternion(m_vehicle_body);
    QQuaternion body (q[0], q[1], q[2], q[3]);

    dBodyID ba = dGeomGetBody(ga), bb = dGeomGetBody(gb);
    if (m_wheel_bodies.contains(ba))
        wheel_index = m_wheel_bodies.indexOf(ba);
    else
        wheel_index = m_wheel_bodies.indexOf(bb);

    if (wheel_index <= 1)
    {
        dReal a = dJointGetHinge2Param(m_wheels[0], dParamLoStop1);
        body *= QQuaternion::fromAxisAndAngle(0, 1, 0, (a / M_PI) * 180.0);
    }

    QVector3D v = {0, 0, 1};
    v = body.rotatedVector(v);

    for (int i = 0; i < MAX_CONTACTS; i++)
    {
        contacts[i].surface.mode = dContactSoftCFM | dContactSoftERP
                | dContactSlip1 | dContactSlip2 | dContactFDir1;
        contacts[i].surface.mu = dInfinity;
        contacts[i].surface.soft_cfm = 5e-4;
        contacts[i].surface.soft_erp = 0.5;
        contacts[i].fdir1[0] = v.x();
        contacts[i].fdir1[1] = 0;
        contacts[i].fdir1[2] = v.z();
        contacts[i].surface.slip1 = 0.01;
        contacts[i].surface.slip2 = .0003;
    }

    int nc = dCollide(ga, gb, MAX_CONTACTS, &contacts[0].geom, sizeof(dContact));
    for (int i = 0; i < nc; i++)
    {
        dJointID c = dJointCreateContact (m_world, m_contact_group,
                                          &contacts[i]);
        dJointAttach (c, dGeomGetBody(ga), dGeomGetBody(gb));
    }
}

void PhysicsSimulation::nearCallbackWrapper(void * i, dGeomID a, dGeomID b)
{
    static_cast<PhysicsSimulation *>(i)->nearCallback(nullptr, a, b);
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
