#ifndef PHYSICS_SIMULATION_H
#define PHYSICS_SIMULATION_H

#include <BulletWorldImporter/btBulletWorldImporter.h>
#include <BulletCollision/btBulletCollisionCommon.h>
#include <BulletDynamics/btBulletDynamicsCommon.h>

#include <QObject>
#include <QHash>
#include <QDir>
#include <QVector3D>

#include <config.h>
#include <common.h>
#include <libtrack/track_model.h>

namespace tl = track_library;

class PhysicsSimulation : public QObject
{
    Q_OBJECT
public:
    PhysicsSimulation(const track_library::TrackModel & model);
    ~PhysicsSimulation();

    btScalar getFrameDuration();
    void setFrameDuration(btScalar duration);
    void initiateSimulation();
public slots:
    void onModelResponse(const DataSet & data);
signals:
    void simulationResponse(const DataSet & data);
private:
    void importBulletFile(const QString & path);
    btRigidBody * createBodyByName(const QString & name);
    void buildTrack();
    void createVehicle();

    btScalar getAbsoluteVelocity(const btRigidBody * body);

    static constexpr btScalar SIMULATION_STEP = 0.001;

    /*
     * potential design flaw: will not work for multi-element/multi-body
     * .bullet files
     */
    QHash<QString, btRigidBody::btRigidBodyConstructionInfo> m_construction_info;

    btRigidBody * m_v_chasis;
    btRigidBody * m_v_wheel_shape;
    btDefaultVehicleRaycaster * m_v_raycaster;
    btRaycastVehicle * m_vehicle;

    btVector3 m_start_location;
    btVector3 m_start_direction;

    const track_library::TrackModel & m_track_model;
    btScalar m_frame_duration;
    btBroadphaseInterface * m_broadphase;
    btDefaultCollisionConfiguration * m_collision_configuration;
    btCollisionDispatcher * m_collision_dispatcher;
    btConstraintSolver * m_constraint_solver;
    btDiscreteDynamicsWorld * m_world;
    btBulletWorldImporter * m_world_importer;
};

#endif
