#ifndef PHYSICS_SIMULATION_H
#define PHYSICS_SIMULATION_H

#include <cmath>

#include <ode/ode.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include <QObject>
#include <QHash>
#include <QDir>
#include <QVector3D>
#include <QPair>

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

    dReal getFrameDuration();
    void setFrameDuration(dReal duration);
    DataSet onModelResponse(const DataSet & data);
private:
    void importModel(const QString & path, const QString & name);
    void buildTrack();
    void createVehicle();
    void nearCallback(void *, dGeomID a, dGeomID b);
    static void nearCallbackWrapper(void * i, dGeomID a, dGeomID b);

    static constexpr dReal GRAVITY_CONSTANT = -9.81;
    static constexpr dReal WORLD_STEP = 1e-3;
    static constexpr int MAX_CONTACTS = 32;

    const track_library::TrackModel & m_track_model;
    dReal m_frame_duration;
    const dReal * m_start_position;
    const dReal * m_start_direction;

    dGeomID m_vehicle_geom;
    dBodyID m_vehicle_body;
    // front left, front right, rear left, rear right
    dJointID m_wheels[4];
    QVector<dGeomID> m_track_geoms;

    dWorldID m_world;
    dSpaceID m_space;
    dJointGroupID m_contact_group;
    QHash<QString, dTriMeshDataID> m_trimesh_data;

    QVector<QPair<float *, int *>> m_allocated_memory;

    Assimp::Importer m_importer;
};

#endif
