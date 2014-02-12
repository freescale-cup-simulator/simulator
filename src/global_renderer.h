#ifndef GLOBAL_RENDERER_H
#define GLOBAL_RENDERER_H

#include <QCoreApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#include <QSemaphore>
#include <QList>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QUrl>

#include <ogre_engine.h>
#include <camera_grabber.h>
#include <camera.h>
#include <config.h>
#include <common.h>
#include <track_model.h>
#include <camera_simulator.h>
#include <QMutex>

using namespace track_library;

class CameraSimulator;
class GlobalRenderer : public QQmlApplicationEngine
{
    Q_OBJECT

public:

    explicit GlobalRenderer(QWindow *parent = 0);
    ~GlobalRenderer();

    void setTrackModel(TrackModel * model);
    void process(DataSet & data);
    OgreEngine * getOgreEngine();
    Ogre::SceneManager * getSceneManager();
    QQuickWindow * getQuickWindow(){return m_root_window;}

signals:
    void startSimulation();
    void ogreInitialized();
    void queryExit();
    void contextObjectsSet();

public slots:

    void initializeOgre();
    void onStatusChanged(QQuickView::Status status);
    void updateScene();

private slots:
    void setContextObjects();

protected:
    bool event(QEvent *event);
private:

    Camera * m_user_camera;
    OgreBites::SdkCameraMan * m_camera_controller;
    OgreEngine * m_ogre_engine;
    Ogre::SceneManager * m_scene_manager;
    Ogre::Root * m_root;
    Ogre::SceneNode * m_car_body;
    Ogre::SceneNode * m_wheels[4];
    TrackModel * m_track_model;
    DataSet m_local_dataset;
    QMutex m_local_dataset_locker;
    bool m_closing;
    QQuickWindow * m_root_window;
};

#endif
