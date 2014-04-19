#ifndef SCENE_H
#define SCENE_H

#include <cmath>
#include <ode/ode.h>
#include <QObject>
#include <QList>
#include <QUrl>
#include <QHash>

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
signals:
    void startMoved(Ogre::Vector3 pos, Ogre::Quaternion rot);
private:
    void cleanup();

    track_library::TrackModel m_track;
    AssetFactory * m_asset_factory;
    QList<Asset *> m_tile_assets;
};

#endif
