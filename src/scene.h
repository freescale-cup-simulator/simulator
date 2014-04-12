#ifndef SCENE_H
#define SCENE_H

#include <cmath>
#include <ode/ode.h>
#include <QObject>
#include <QList>
#include <QUrl>

#include <global_renderer.h>
#include <asset_factory.h>
#include <ogre_engine.h>
#include <track_io.h>
#include <track_model.h>

namespace tlio = track_library::io;

class Scene : public QObject
{
    Q_OBJECT
public:
    Scene(AssetFactory * asset_factory, QObject * parent = nullptr);
    ~Scene();
public slots:
    bool loadTrack(const QUrl &file);
private:

    void cleanup();

    track_library::TrackModel m_track;
//    Ogre::SceneManager * m_scene_manager;
    AssetFactory * m_asset_factory;
    QList<Asset *> m_tile_assets;
    QUrl m_file_url;
    Ogre::Vector3 * m_start_position;
    Ogre::Quaternion m_start_rotation_q;
};

#endif
