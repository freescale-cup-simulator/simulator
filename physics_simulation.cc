#include <physics_simulation.h>

namespace {

static inline QVector3D btv2qv (const btVector3 & v)
{
    return QVector3D (v.getX(), v.getY(), v.getZ());
}

}

PhysicsSimulation::PhysicsSimulation(const track_library::TrackModel &model)
    : m_start_direction(btVector3(0, 1, 0))
    , m_track_model(model)
    , m_frame_duration(0.01)
    , m_broadphase(new btDbvtBroadphase)
    , m_collision_configuration(new btDefaultCollisionConfiguration)
    , m_collision_dispatcher(new btCollisionDispatcher(m_collision_configuration))
    , m_constraint_solver(new btSequentialImpulseConstraintSolver)
    , m_world(new btDiscreteDynamicsWorld(m_collision_dispatcher, m_broadphase,
                                          m_constraint_solver,
                                          m_collision_configuration))
    , m_world_importer(new btBulletWorldImporter)
{
    QDir objects(QString(RESOURCE_DIRECTORY "bullet/"), "*.bullet", QDir::Name,
                 QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    auto entries = objects.entryList();
    Q_ASSERT(!entries.isEmpty());
    for (QString file : entries)
        importBulletFile(objects.path() + "/" + file);

    m_collision_configuration->setConvexConvexMultipointIterations(4, 4);
    m_world->setGravity(btVector3(0, 0, -9.81));
    buildTrack();
    createVehicle();

    qDebug(" -- Physics simulation initialized with %d shapes",
           m_construction_info.count());

    m_v_chasis->translate(m_start_location);
}

PhysicsSimulation::~PhysicsSimulation()
{
    m_world_importer->deleteAllData();
    delete m_world_importer;
    delete m_world;
    delete m_constraint_solver;
    delete m_collision_dispatcher;
    delete m_collision_configuration;
    delete m_broadphase;
}

btScalar
PhysicsSimulation::getFrameDuration()
{
    return m_frame_duration;
}

void
PhysicsSimulation::setFrameDuration(btScalar duration)
{
    m_frame_duration = duration;
}

void PhysicsSimulation::initiateSimulation()
{
    DataSet s;

    s[CameraPositionX] = m_start_location.getX();
    s[CameraPositionY] = m_start_location.getY();
    s[CameraPositionZ] = m_start_location.getZ();
    s[CameraRotationX] = 75;
    s[CameraRotationY] = 0;
    s[CameraRotationZ] = 0;

    simulationResponse(s);
}

void
PhysicsSimulation::onModelResponse(const DataSet & control)
{
    static btScalar speed_multiplier = 2;

    m_vehicle->applyEngineForce(control[WheelSpeedL].toFloat() * speed_multiplier, 2);
    m_vehicle->applyEngineForce(control[WheelSpeedR].toFloat() * speed_multiplier, 3);
    m_vehicle->setBrake(0, 2);
    m_vehicle->setBrake(0, 3);
    m_vehicle->setSteeringValue(control[MovementAngle].toFloat(), 0);
    m_vehicle->setSteeringValue(control[MovementAngle].toFloat(), 1);

    for (int i = 0; i < m_vehicle->getNumWheels(); i++)
        m_vehicle->updateWheelTransform(i);
    m_world->stepSimulation(m_frame_duration, m_frame_duration / SIMULATION_STEP,
                            SIMULATION_STEP);

    const btTransform & wt = m_v_chasis->getWorldTransform();
    btVector3 camera_position = wt.getOrigin() + m_vehicle->getForwardVector();
    btScalar ca = m_vehicle->getForwardVector().angle(btVector3(0, 1, 0))
            * SIMD_DEGS_PER_RAD;
    DataSet response;

    response[CameraPositionX] = camera_position.getX();
    response[CameraPositionY] = camera_position.getY();
    response[CameraPositionZ] = camera_position.getZ() + 0.6;
    response[CameraRotationX] = 65;
    response[CameraRotationY] = ca;
    response[CameraRotationZ] = 0;

    simulationResponse(response);

#if 0
    static int i = 0;
    if (i++ % 5 == 0)
        qDebug("%.2f %.2f %.2f at %.2f, z rot %.2f rad", x, y, z, v,
               wt.getRotation().getZ());
#endif
}

void
PhysicsSimulation::importBulletFile(const QString &path)
{
    qDebug("Importing %s", path.toLocal8Bit().data());
    m_world_importer->loadFile(path.toLocal8Bit().data());
    int index = m_world_importer->getNumRigidBodies();
    btCollisionObject * co = m_world_importer->getRigidBodyByIndex(index - 1);
    btRigidBody * p = btRigidBody::upcast(co);

    btRigidBody::btRigidBodyConstructionInfo info (p->getInvMass(), nullptr,
                                                   p->getCollisionShape(),
                                                   btVector3(0, 0, 0));
    m_construction_info.insert(m_world_importer->getNameForPointer(co), info);
}

inline btRigidBody *
PhysicsSimulation::createBodyByName(const QString &name)
{
    auto i = m_construction_info.find(name);
    if (i == m_construction_info.end())
        return nullptr;
    btRigidBody * p = new btRigidBody (*i);
    m_world->addRigidBody(p);
    return p;
}

void
PhysicsSimulation::buildTrack()
{
    btRigidBody * p;
    btQuaternion rq;
    btMatrix3x3 rm;

    for (const tl::Tile t : m_track_model.tiles())
    {
        bool is_hill = t.type() == tl::Tile::Hill;
        btVector3 location = btVector3(t.x() + 0.5, t.y() + 0.5, 0);

        p = createBodyByName(is_hill ? "hill" : "tile");
        p->translate(location + btVector3(0, 0, is_hill ? -0.98 : -0.05));
        rm.setEulerZYX(0, is_hill ? M_PI_2 : 0, t.rotation());
        rm.getRotation(rq);
        p->getWorldTransform().setRotation(rq);

        if (t.type() == tl::Tile::Start)
        {
            m_start_location = location + btVector3(0, 0, 1);
            m_start_direction
                    = m_start_direction.rotate(btVector3(0, 0, 0.2),
                                               (t.rotation() / 180) * M_PI);

            qDebug("start location: (%.2f,%.2f), direction: (%.0f,%.0f,%.0f)",
                   location.getX(), location.getY(),
                   m_start_direction.getX(),
                   m_start_direction.getY(),
                   m_start_direction.getZ());
        }
    }
}

void PhysicsSimulation::createVehicle()
{
    static btRaycastVehicle::btVehicleTuning tuning;

    const btScalar vw = 0.08, vl = 0.12, vh = 0.05;
    const btScalar wheel_radius = 4 * vh;
    const btScalar connection_height = wheel_radius / 2;
    const btScalar mass = 2;

    btVector3 connection_point;
    btVector3 wheel_direction(0, 0, -1);
    btVector3 wheel_axis(1, 0, 0);

    btCollisionShape * s = new btBoxShape(btVector3(vw, vl, vh));
    btCompoundShape * cs = new btCompoundShape;
    btTransform lt;
    btVector3 inertia;
    cs->setMargin(0);

    lt.setIdentity();
    lt.setOrigin(btVector3(0, 0, 1));
    cs->addChildShape(lt, s);
    cs->calculateLocalInertia(mass, inertia);

    m_v_chasis = new btRigidBody(mass, new btDefaultMotionState, cs, inertia);
    m_v_chasis->setActivationState(DISABLE_DEACTIVATION);
    m_world->addRigidBody(m_v_chasis);

    m_v_raycaster = new btDefaultVehicleRaycaster(m_world);
    m_vehicle = new btRaycastVehicle(tuning, m_v_chasis, m_v_raycaster);

    m_world->addVehicle(m_vehicle);
    m_vehicle->setCoordinateSystem(0, 2, 1);

    //front left
    connection_point = btVector3(-vw - 0.05, vl, connection_height);
    m_vehicle->addWheel(connection_point, wheel_direction, wheel_axis,
                        connection_height, wheel_radius, tuning, true);

    //front right
    connection_point = btVector3(vw + 0.05, vl, connection_height);
    m_vehicle->addWheel(connection_point, wheel_direction, wheel_axis,
                        connection_height, wheel_radius, tuning, true);


    //rear left
    connection_point = btVector3(-vw - 0.05, -vl, connection_height);
    m_vehicle->addWheel(connection_point, wheel_direction, wheel_axis,
                        connection_height, wheel_radius, tuning, false);

    //rear right
    connection_point = btVector3(vw + 0.05, -vl, connection_height);
    m_vehicle->addWheel(connection_point, wheel_direction, wheel_axis,
                        connection_height, wheel_radius, tuning, false);

    for (int i = 0; i < m_vehicle->getNumWheels(); i++)
    {
        btWheelInfo & wheel = m_vehicle->getWheelInfo(i);
        wheel.m_suspensionStiffness = 20.0;
        wheel.m_wheelsDampingRelaxation = 2.3;
        wheel.m_wheelsDampingCompression = 4.5;
        wheel.m_frictionSlip = 1000;
        wheel.m_rollInfluence = 0.1;
    }
}

inline btScalar
PhysicsSimulation::getAbsoluteVelocity(const btRigidBody *body)
{
    btVector3 vel = body->getVelocityInLocalPoint(btVector3(0, 0, 0));
    return vel.length();
}
