#include <physics_simulation.h>

PhysicsSimulation::PhysicsSimulation(const track_library::TrackModel &model)
    : m_track_model(model)
    , m_start_direction(btVector3(0, 1, 0))
    , m_frame_duration(0.1)
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

    m_world->setGravity(btVector3(0, 0, -9.81));
    buildTrack();

    qDebug(" -- Physics simulation initialized with %d shapes",
           m_construction_info.count());

    m_sphere = createBodyByName("bt_sphere");
    m_sphere->translate(m_start_location);
    m_sphere->setAngularVelocity(btVector3(0, 3000, 0));
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

void
PhysicsSimulation::onModelResponse(const DataSet &)
{
    m_world->stepSimulation(m_frame_duration);
    btTransform wt = m_sphere->getWorldTransform();
    btVector3 p = wt.getOrigin();
    btScalar x = p.getX(), y = p.getY(), z = p.getZ();
    btScalar v = getAbsoluteVelocity(m_sphere);
//    if (i++ % 5 == 0)
//        qDebug("%f %f %f at %f", x, y, z, v);
//    qDebug("%f %f %f", x, y, z);
    // useful for plotting output
    qDebug("%.3f %.3f", x, z);
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

    const int tmh = m_track_model.height() - 1;

    for (const tl::Tile t : m_track_model.tiles())
    {
        int x = t.x(), y = t.y();
        btVector3 location = btVector3(x + 0.5, tmh - y + 0.5, 0);

        if (t.type() == tl::Tile::Hill)
        {
            p = createBodyByName("bt_hill");
            p->translate(location + btVector3(0, 0, -0.3));
        }
        else
        {
            p = createBodyByName("bt_tile");
            p->translate(location);
        }

        rm.setEulerYPR(t.rotation(), 0, 0);
        rm.getRotation(rq);
        p->getWorldTransform().setRotation(rq);

        if (t.type() == tl::Tile::Start)
        {
            m_start_location = location + btVector3(0, 0, 0.4);
            m_start_direction
                    = m_start_direction.rotate(btVector3(0, 0, 1),
                                               (t.rotation() / 180) * M_PI);

            qDebug("start location: (%.2f,%.2f), direction: (%.0f,%.0f,%.0f)",
                   location.getX(), location.getY(),
                   m_start_direction.getX(),
                   m_start_direction.getY(),
                   m_start_direction.getZ());
        }
    }
}

inline btScalar
PhysicsSimulation::getAbsoluteVelocity(const btRigidBody *body)
{
    btVector3 vel = body->getVelocityInLocalPoint(btVector3(0, 0, 0));
    return std::sqrt(std::pow(vel.getX(), 2) + std::pow(vel.getY(), 2)
                     + std::pow(vel.getZ(), 2));
}
