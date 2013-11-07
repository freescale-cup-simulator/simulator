#ifndef GLOBAL_RENDERER_H
#define GLOBAL_RENDERER_H

#include <QCoreApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#include <QSemaphore>
#include <QList>

#include <ogre_engine.h>
#include <camera_grabber.h>
#include <camera.h>
#include <shared_image.h>
#include <config.h>
#include <common.h>
#include <track_model.h>

using namespace track_library;

class GlobalRenderer : public QQuickView
{
    Q_OBJECT

public:

    explicit GlobalRenderer(QWindow *parent = 0);
    ~GlobalRenderer();

    void setTrackModel(TrackModel * model);
    CameraGrabber * createCameraGrabber(QSemaphore * sync);
    void process(DataSet & data);
    void attachCamToGUI(quint32 index);

signals:
    void startSimulation();
    void ogreInitialized();

public slots:

    void initializeOgre();
    void addContent();
    void onStatusChanged(QQuickView::Status status);

private:

    Camera * m_user_camera;
    OgreBites::SdkCameraMan * m_camera_controller;
    OgreEngine * m_ogre_engine;
    Ogre::SceneManager * m_scene_manager;
    Ogre::Root * m_root;
    TrackModel * m_track_model;
    QList<CameraGrabber *> m_camera_grabbers;
    Ogre::SceneNode * m_ogre_head;
};

#endif