#ifndef PHYSICS_SIMULATION_H
#define PHYSICS_SIMULATION_H

#include <cmath>

#include <ode/ode.h>

#include <OGRE/OgreMesh.h>
#include <OGRE/OgreSubMesh.h>
#include <OGRE/OgreBone.h>
#include <OGRE/OgreVertexIndexData.h>
#include <OGRE/OgreMeshManager.h>

#include <QObject>
#include <QHash>
#include <QVector3D>
#include <QPair>

#include <config.h>
#include <common.h>
#include <libtrack/track_model.h>
#include <property.h>
#include <property_model.h>

namespace tl = track_library;

class PhysicsSimulation : public QObject
{
    Q_OBJECT
public:
    PhysicsSimulation(const track_library::TrackModel & model,
                      QObject * parent = nullptr);
    ~PhysicsSimulation();

    void process(DataSet & data);
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

    void importModel(const QString & mesh_name, const QString & name);
    void buildTrack();
    void createVehicle();
    void nearCallback(void *, dGeomID a, dGeomID b);
    static void nearCallbackWrapper(void * i, dGeomID a, dGeomID b);
    void getOgreMeshData(const Ogre::Mesh * const mesh, size_t & vertex_count,
                         float * & vertices, size_t & index_count,
                         unsigned int * & indices);
    void updateBodyData(DataSet & d);

    static constexpr dReal GRAVITY_CONSTANT = -9.81;
    static constexpr int MAX_CONTACTS = 128;

    const track_library::TrackModel & m_track_model;
    const dReal * m_start_position;
    dQuaternion m_start_rotation_q;
    StartDirection m_start_direction;

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
};

#endif
