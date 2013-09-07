#include <physics_simulation.h>

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

    dBodyID id = dBodyCreate(m_world);
    m_sphere = dCreateSphere(m_space, 0.5);
    static dMass m;
    dMassSetSphereTotal(&m, 2, 0.5);
    dBodySetMass(id, &m);
    dGeomSetBody(m_sphere, id);
    dBodySetPosition(id, m_start_position[0], m_start_position[1], 15.5);
}

PhysicsSimulation::~PhysicsSimulation()
{
    dJointGroupDestroy(m_contact_group);
    dSpaceDestroy(m_space);
    dWorldDestroy(m_world);
    dCloseODE();
    for (void * p : m_allocated_memory)
        delete p;
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
    dSpaceCollide(m_space, this, &PhysicsSimulation::nearCallbackWrapper);
    dWorldStep(m_world, 0.05);
    dJointGroupEmpty(m_contact_group);
    const dReal * p = dGeomGetPosition(m_sphere);
    qDebug("%f,%f,%f", p[0], p[1], p[2]);

    DataSet s;

    s[CameraPositionX] = p[0];
    s[CameraPositionY] = p[1];
    s[CameraPositionZ] = p[2] + 1;
    s[CameraRotationX] = 90;
    s[CameraRotationY] = 0;
    s[CameraRotationZ] = 0;

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
// FIXME: ASSIMP bug, mesh names not set
//    for (int i = 0; i < s->mNumMeshes; i++)
//    {
//        qDebug("Mesh name: %s", s->mMeshes[i]->mName.C_Str());
//    }
    const aiMesh * m = s->mMeshes[0];
    dVector3 * vertices = new dVector3[m->mNumVertices * 3];
    m_allocated_memory.append(vertices);
    qDebug("allocated %p", vertices);
    for (unsigned int i = 0; i < m->mNumVertices; i++)
    {
        vertices[i][0] = m->mVertices[i].x;
        vertices[i][1] = m->mVertices[i].y;
        vertices[i][2] = m->mVertices[i].z;
    }

    unsigned int * indices = new unsigned int[m->mNumFaces * 3];
    m_allocated_memory.append(indices);
    qDebug("allocated %p", indices);
    for (unsigned int i = 0; i < m->mNumFaces; i++)
    {
        indices[i * 3]     = m->mFaces[i].mIndices[0];
        indices[i * 3 + 1] = m->mFaces[i].mIndices[1];
        indices[i * 3 + 2] = m->mFaces[i].mIndices[2];
    }

    dTriMeshDataID id = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildDouble(id, vertices, sizeof(dVector3), m->mNumVertices,
                                indices, m->mNumFaces * 3, sizeof(unsigned int));
    m_trimesh_data.insert(name, id);
    qDebug("Saved dTriMeshDataID = %p", static_cast<void *>(id));
}

void
PhysicsSimulation::buildTrack()
{
    for (const tl::Tile & tile : m_track_model.tiles())
    {
        dGeomID id = dCreateTriMesh(m_space, m_trimesh_data["tile"], 0, 0, 0);
        dGeomSetBody(id, 0);
        dGeomSetPosition(id, tile.x() + 0.5, tile.y() + 0.5, 0);

        dMatrix3 rm;
        dRFromEulerAngles(rm, 0, 0,  (tile.rotation() / 180) * M_PI);
        dGeomSetRotation(id, rm);

        if (tile.type() == tl::Tile::Start)
        {
            m_start_position = dGeomGetPosition(id);
            m_start_direction = dGeomGetRotation(id);
        }
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
}

void PhysicsSimulation::nearCallback(void *, dGeomID ga, dGeomID gb)
{
    dBodyID a = dGeomGetBody(ga);
    dBodyID b = dGeomGetBody(gb);
    dContact contact;

    contact.surface.mode = dContactBounce;
    // friction parameter
    contact.surface.mu = dInfinity;
    // bounce is the amount of "bouncyness".
    contact.surface.bounce = 0.96;
    // bounce_vel is the minimum incoming velocity to cause a bounce
    contact.surface.bounce_vel = 0.01;
    // constraint force mixing parameter
    contact.surface.soft_cfm = 1;

    int nc = dCollide(ga, gb, 1, &contact.geom, sizeof(dContact));

    if (nc)
    {
        dJointID c = dJointCreateContact (m_world, m_contact_group, &contact);
        dJointAttach (c, a, b);
    }
}

void PhysicsSimulation::nearCallbackWrapper(void * i, dGeomID a, dGeomID b)
{
    static_cast<PhysicsSimulation *>(i)->nearCallback(nullptr, a, b);
}
