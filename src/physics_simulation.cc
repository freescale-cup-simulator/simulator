#include <physics_simulation.h>

PhysicsSimulation::PhysicsSimulation(QObject * parent)
    : QObject(parent)
    , m_world(0)
    , m_space(0)
    , m_contact_group(0)

{
    dInitODE();
    m_world = dWorldCreate();
    m_space = dSimpleSpaceCreate(nullptr);
    m_contact_group = dJointGroupCreate(0);
    dWorldSetGravity(m_world, 0, GRAVITY_CONSTANT, 0);
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

void PhysicsSimulation::process(DataSet & data)
{
    dSpaceCollide(m_space, this, &PhysicsSimulation::nearCallbackWrapper);
    dWorldStep(m_world, data.physics_timestep);
    dJointGroupEmpty(m_contact_group);
}

void PhysicsSimulation::nearCallback(void *, dGeomID ga, dGeomID gb)
{
    /*
     * collision categories:
     * track is cat 0, collides with cat 1, 2
     * vehicle chasis is cat 1, collides with cat 0
     * wheels are cat 2, collides with cat 0
     */

    auto cat1 = dGeomGetCategoryBits (ga);
    auto cat2 = dGeomGetCategoryBits (gb);
    auto col1 = dGeomGetCollideBits (ga);
    auto col2 = dGeomGetCollideBits (gb);

    if (((cat1 & col2) == 0) && ((cat2 & col1) == 0))
        return;

    static dSurfaceParameters default_surface =
    {
        dContactSoftCFM | dContactSoftERP, // mode
        0, 0, 0, 0, 0, 0, 0, // up to erp
        0.3, // erp
        0.01, // cfm
        0, 0, 0, 0, 0
    };

    dContact contacts[MAX_CONTACTS];
    dBodyID ba = dGeomGetBody(ga), bb = dGeomGetBody(gb);
    dContact contact;
    void * userdata1 = nullptr, * userdata2 = nullptr;

    if (ba)
        userdata1 = dBodyGetData(ba);
    if (bb)
        userdata2 = dBodyGetData(bb);

    Asset * asset1 = reinterpret_cast<Asset *>(userdata1);
    Asset * asset2 = reinterpret_cast<Asset *>(userdata2);

    if (asset1 && asset1->has_contact)
        contact = asset1->contact;
    else if (asset2 && asset2->has_contact)
        contact = asset2->contact;
    else
        contact.surface = default_surface;

    for (int i = 0; i < MAX_CONTACTS; i++)
        contacts[i] = contact;

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
