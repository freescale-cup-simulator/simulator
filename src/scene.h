#ifndef SCENE_H
#define SCENE_H

#include <cmath>
#include <ode/ode.h>

#include <QObject>

#include <mesh_manager.h>
#include <ogre_engine.h>
#include <track_model.h>

class Scene : public QObject
{
public:
    Scene(QObject * parent);
    static Scene * init(MeshManager * mesh_manager,OgreEngine * engine, track_library::TrackModel * track_model);
private:
    dWorldID m_world;
    dSpaceID m_space;
    QVector<dGeomID> m_track_geoms;
};

#endif
