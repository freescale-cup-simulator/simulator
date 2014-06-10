#ifndef PHYSICS_SIMULATION_H
#define PHYSICS_SIMULATION_H

#include <ode/ode.h>

#include <QObject>
#include <QDebug>

#include <common.h>
#include <asset.h>

/*!
 * \brief Performs step-by-step simulation of physical interaction of assets
 */
class PhysicsSimulation : public QObject
{
    Q_OBJECT
public:
    PhysicsSimulation(QObject * parent = nullptr);
    ~PhysicsSimulation();

    inline dWorldID world() { return m_world; }
    inline dSpaceID space() { return m_space; }
    void process(DataSet & data);
private:
    void nearCallback(void *, dGeomID a, dGeomID b);
    static void nearCallbackWrapper(void * i, dGeomID a, dGeomID b);

    static constexpr dReal GRAVITY_CONSTANT = -9.81;
    static constexpr int MAX_CONTACTS = 256;

    dWorldID m_world;
    dSpaceID m_space;
    dJointGroupID m_contact_group;
};

#endif
