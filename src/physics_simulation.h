#ifndef PHYSICS_SIMULATION_H
#define PHYSICS_SIMULATION_H

#include <cmath>

#include <ode/ode.h>

#include <QObject>
#include <QHash>
#include <QVector3D>
#include <QPair>

#include <config.h>
#include <common.h>
#include <libtrack/track_model.h>

namespace tl = track_library;

class PhysicsSimulation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal vehicleVelocity READ vehicleVelocity NOTIFY vehicleVelocityChanged)

    Q_PROPERTY(qreal slip1 READ slip1 WRITE setSlip1 NOTIFY slip1Changed)
    Q_PROPERTY(qreal slip2 READ slip2 WRITE setSlip2 NOTIFY slip2Changed)
    Q_PROPERTY(qreal rho READ rho WRITE setRho NOTIFY rhoChanged)
    Q_PROPERTY(qreal mu READ mu WRITE setMu NOTIFY muChanged)

    Q_PROPERTY(qreal suspension_k READ suspension_k WRITE setSuspension_k
               NOTIFY suspension_k_Changed)
    Q_PROPERTY(qreal suspension_damping READ suspension_damping
               WRITE setSuspension_damping NOTIFY suspension_k_Changed)

    Q_PROPERTY(qreal tire_k READ tire_k WRITE setTire_k NOTIFY tire_k_Changed)
    Q_PROPERTY(qreal tire_damping READ tire_damping
               WRITE setTire_damping NOTIFY tire_k_Changed)
public:
    PhysicsSimulation(QObject * parent = nullptr);
    ~PhysicsSimulation();
    inline dWorldID world(){
        return m_world;
    }
    inline dSpaceID space(){
        return m_space;
    }

    void process(DataSet & data);
    qreal vehicleVelocity();

    qreal slip1() const { return m_slip1; }
    qreal slip2() const { return m_slip2; }
    qreal rho() const { return m_rho; }
    qreal suspension_k() const { return m_suspension_k; }
    qreal suspension_damping() const { return m_suspension_damping; }
    qreal tire_k() const { return m_tire_k; }
    qreal tire_damping() const { return m_tire_damping; }

    qreal mu() const
    {
        return m_mu;
    }

public slots:
    void createWorld();

    void setSlip1(qreal arg) { m_slip1 = arg; emit slip1Changed(); }
    void setSlip2(qreal arg) { m_slip2 = arg; emit slip2Changed(); }
    void setRho(qreal arg) { m_rho = arg; emit rhoChanged(); }
    void setMu(qreal arg) { m_mu = arg; emit muChanged(); }

    void setSuspension_k(qreal arg)
    {
        m_suspension_k = arg;
        emit suspension_k_Changed();
    }

    void setSuspension_damping(qreal arg)
    {
        m_suspension_damping = arg;
        emit suspension_k_Changed();
    }

    void setTire_k(qreal arg) { m_tire_k = arg; emit tire_k_Changed(); }
    void setTire_damping(qreal arg) { m_tire_damping = arg; emit tire_k_Changed(); }

signals:
    void vehicleVelocityChanged();
    void slip1Changed();
    void slip2Changed();
    void rhoChanged();
    void suspension_k_Changed();
    void tire_k_Changed();
    void muChanged();

private:
    enum StartDirection {Up = 0, Right, Down, Left};

    inline void setTrimeshLastTransform(dGeomID id)
    {
        const dReal * pos = dGeomGetPosition(m_vehicle_geom);
        const dReal * rot = dGeomGetRotation(m_vehicle_geom);
        dMatrix4 transform
        {
            rot[0], rot[4], rot[8],  0,
            rot[1], rot[5], rot[9],  0,
            rot[2], rot[6], rot[10], 0,
            pos[0], pos[1], pos[2],  1
        };
        dGeomTriMeshSetLastTransform(id, transform);
    }
    void buildTrack();
    void createVehicle();
    void nearCallback(void *, dGeomID a, dGeomID b);
    static void nearCallbackWrapper(void * i, dGeomID a, dGeomID b);
    void updateBodyData(DataSet & d);
    void updateERPandCFM(const DataSet & d);

    static constexpr dReal GRAVITY_CONSTANT = -9.81;
    static constexpr int MAX_CONTACTS = 256;

    const dReal * m_start_position;
    dQuaternion m_start_rotation_q;
    QVector3D m_start_rotation_v;

    dGeomID m_vehicle_geom;
    dBodyID m_vehicle_body;
    dBodyID m_camera_body;
    dJointID m_wheels[4];
    QVector<dBodyID> m_wheel_bodies;
    QVector<dGeomID> m_track_geoms;

    dWorldID m_world;
    dSpaceID m_space;
    dJointGroupID m_contact_group;

    QHash<QString, dTriMeshDataID> m_trimesh_data;

    QVector<QPair<float *, unsigned int *>> m_allocated_memory;

    qreal m_vehicleVelocity;
    dReal m_surfaceERP;
    dReal m_surfaceCFM;

    qreal m_slip1;
    qreal m_slip2;
    qreal m_mu;
    qreal m_rho;
    qreal m_suspension_k;
    qreal m_suspension_damping;
    qreal m_tire_k;
    qreal m_tire_damping;
};

#endif
