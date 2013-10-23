#ifndef GLOBAL_RENDERER_H
#define GLOBAL_RENDERER_H

#include <QCoreApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>

#include <QList>

#include <ogre_engine.h>
#include <camera_grabber.h>
#include <camera.h>
#include <shared_image.h>
#include <config.h>
#include <track_model.h>

using namespace track_library;

class GlobalRenderer : public QQuickView
{
    Q_OBJECT

public:

    explicit GlobalRenderer(TrackModel * model,QWindow *parent = 0);
    ~GlobalRenderer();

    SharedImage * createCameraGrabber();


signals:

    void ogreInitialized();
    void startSimulation();

public slots:

    void initializeOgre();
    void addContent();
    void onStatusChanged(QQuickView::Status status);
    void onFrameSwapped();

private:

    Camera * m_user_camera;
    OgreBites::SdkCameraMan * m_camera_controller;
    OgreEngine * m_ogre_engine;
    Ogre::SceneManager * m_scene_manager;
    Ogre::Root * m_root;
    TrackModel * m_track_model;
    QList<SharedImage *> m_shared_images;
};

#endif
