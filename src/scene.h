#ifndef SCENE_H
#define SCENE_H

#include <cmath>
#include <ode/ode.h>
#include <QObject>

#include <asset_manager.h>
#include <ogre_engine.h>
#include <track_io.h>
#include <track_model.h>

class Scene : public QObject
{
    Q_OBJECT
public:
    Scene(Ogre::SceneManager * manager,QObject * parent=0);
public slots:
    bool loadTrack(const QString & file);
    void redraw();
private:
    track_library::TrackModel m_track;
    Ogre::SceneManager * m_scene_manager;
};

#endif
