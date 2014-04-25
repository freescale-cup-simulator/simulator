#include <vehicle.h>

Vehicle::Vehicle(QObject * parent)
    : QObject(parent)
    , m_vehicleVelocity(0)
    , m_slip1(0.00001)
    , m_slip2(0.0002)
    , m_mu(1000)
    , m_rho(0.01)
    , m_suspension_k(150000)
    , m_suspension_damping(200)
    , m_tire_k(30000)
    , m_tire_damping(400)
{}

Vehicle::~Vehicle()
{
    destroy();
}

void Vehicle::create(Ogre::Vector3 pos, Ogre::Quaternion rot)
{
    constexpr dReal mass = 2;
    constexpr qreal spawn_height = 0.1;

    pos += {0, spawn_height, 0};

    Asset * chasis = m_asset_factory->createAsset(AssetFactory::SceneNode
                                                 | AssetFactory::Body3D
                                                 | AssetFactory::MeshGeometry
                                                 | AssetFactory::PhyBody,
                                                 "car_body.mesh");
    chasis->setMass(mass);
    chasis->setPosition(pos);
    chasis->rotate(rot);

    dGeomSetCategoryBits(chasis->geometry, (1 << 1)); // cat 1
    dGeomSetCollideBits(chasis->geometry, (1 << 0)); // collide with cat 0

    m_parts[PART_CHASIS] = chasis;

    auto skeleton = chasis->mesh->getSkeleton();
    auto world = dBodyGetWorld(chasis->body);
    auto cp = skeleton->getBone("cam")->_getDerivedPosition();

    Asset * camera = m_asset_factory->createAsset(AssetFactory::PhyBody);
    camera->setPosition((rot * cp) + pos);
    camera->rotate(rot);
    {
        auto jid = dJointCreateFixed(world, 0);
        dJointAttach(jid, chasis->body, camera->body);
        dJointSetFixed(jid);
    }
    m_parts[PART_CAMERA] = camera;

    constexpr int nwheels = 4;
    const char * wn[nwheels] = {"wheel_hl", "wheel_hr", "wheel_rl", "wheel_rr"};

    AssetFactory::GeomFunction wheel_f = [](dWorldID, dSpaceID space) -> dGeomID
    {
        constexpr float radius = 0.025;
        return dCreateSphere(space, radius);
    };

    Ogre::Vector3 axis2 = rot * Ogre::Vector3(1, 0, 0);

    for (int i = 0; i < nwheels; i++)
    {
        auto v = skeleton->getBone(wn[i])->_getDerivedPosition();
        auto w = m_asset_factory->createAsset(AssetFactory::SceneNode
                                             | AssetFactory::Body3D
                                             | AssetFactory::PhyBody
                                             | AssetFactory::CustomGeometry,
                                             (i < 2) ? "wheel_h.mesh"
                                                     : "wheel_r.mesh",
                                             wheel_f);

        v = (rot * v) + pos;

        w->setMass(0.5);
        w->setPosition(v);
        w->rotate(rot);

//        dBodySetFiniteRotationMode(w->body, 1);

        auto jid = dJointCreateHinge2(world, 0);
        dJointAttach(jid, chasis->body, w->body);
        dJointSetHinge2Anchor(jid, v.x, v.y, v.z);
        dJointSetHinge2Axis1(jid, 0, 1, 0);
        dJointSetHinge2Axis2(jid, -axis2.x, 0, -axis2.z);
        dJointSetHinge2Param(jid, dParamVel2, 200);

        if (i >= 2)
        {
            dJointSetHinge2Param(jid, dParamLoStop, -1e-3);
            dJointSetHinge2Param(jid, dParamHiStop, 1e-3);
        }

        dGeomSetCategoryBits(w->geometry, (1 << 2)); // cat 2
        dGeomSetCollideBits(w->geometry, (1 << 0)); // collide with 0

        m_parts[PART_WHEEL_FL + i] = w;
        m_joints[PART_WHEEL_FL + i] = jid;
    }
}

void Vehicle::process(DataSet & ds)
{
    updateERPandCFM(ds);
    auto v = util::dv2qv(dBodyGetLinearVel(m_parts[PART_CHASIS]->body)).length();
    auto v_ = m_vehicleVelocity;
    m_vehicleVelocity = v;
    if (v != v_)
        vehicleVelocityChanged();
    updateContact();

    dJointSetHinge2Param(m_joints[PART_WHEEL_RL], dParamFMax2, 1);//ds.wheel_power_l);
    dJointSetHinge2Param(m_joints[PART_WHEEL_RR], dParamFMax2, 1);//ds.wheel_power_r);

    const float rad_angle = (ds.current_wheel_angle / 180.0) * M_PI;
    for (int i = PART_WHEEL_FL; i <= PART_WHEEL_FR; i++)
    {
        dJointSetHinge2Param(m_joints[i], dParamLoStop1, rad_angle * 1.0 - 1e-3);
        dJointSetHinge2Param(m_joints[i], dParamHiStop1, rad_angle * 1.0 + 1e-3);
    }

    ds.camera.position = util::dv2ov(dBodyGetPosition(m_parts[PART_CAMERA]->body));
    ds.camera.rotation = util::dq2oq(dBodyGetQuaternion(m_parts[PART_CAMERA]->body));
}

void Vehicle::startMoved(Ogre::Vector3 pos, Ogre::Quaternion rot)
{
    destroy();
    create(pos, rot);
    m_start_rotation = rot;
}

void Vehicle::simulationStopped()
{
    m_parts[PART_CHASIS]->setVisible(false);
}

void Vehicle::updateERPandCFM(const DataSet &d)
{
    const double dt = d.physics_timestep;

    m_surfaceERP = dt * m_suspension_k / (dt * m_suspension_k + m_suspension_damping);
    m_surfaceCFM = 1.0 / (dt * m_suspension_k + m_suspension_damping);

    dReal wERP = dt * m_tire_k / (dt * m_tire_k + m_tire_damping);
    dReal wCFM = 1.0 / (dt * m_tire_k + m_tire_damping);

    for (auto j : m_joints)
    {
        dJointSetHinge2Param(j, dParamSuspensionERP, wERP);
        dJointSetHinge2Param(j, dParamSuspensionCFM, wCFM);
    }
}

void Vehicle::updateContact()
{
    dBodyID chasis = m_parts[PART_CHASIS]->body;

    for (int i = PART_WHEEL_FL; i <= PART_WHEEL_RR; i++)
    {
        Asset * a = m_parts[i];

        Ogre::Quaternion body(util::dq2oq(dBodyGetQuaternion(chasis)));
        Ogre::Quaternion wheel;

        dReal wheel_a = dJointGetHinge2Param(m_joints[i], dParamLoStop1);
        wheel.FromAngleAxis(Ogre::Radian(-wheel_a), Ogre::Vector3(0, 1, 0));

        Ogre::Vector3 fdir1 = (body * wheel) * Ogre::Vector3(1, 0, 0);
        fdir1.normalise();

        a->contact.surface.mode = dContactSoftCFM | dContactSoftERP
                | dContactSlip1 | dContactSlip2// | dContactFDir1
                | dContactApprox1 | dContactRolling;
        a->contact.surface.mu = m_mu;
        a->contact.surface.soft_cfm = m_surfaceCFM;
        a->contact.surface.soft_erp = m_surfaceERP;
        a->contact.surface.rho = m_rho;
        a->contact.surface.rho2 = m_rho;
        a->contact.surface.rhoN = m_rho;
//        a->contact.fdir1[0] = fdir1.x;
//        a->contact.fdir1[1] = 0;
//        a->contact.fdir1[2] = fdir1.z;
        a->contact.surface.slip1 = m_slip1 * m_vehicleVelocity;
        a->contact.surface.slip2 = m_slip2 * m_vehicleVelocity;

        a->has_contact = true;
    }
}

void Vehicle::destroy()
{
    qDeleteAll(m_parts);
    m_parts.clear();
}

