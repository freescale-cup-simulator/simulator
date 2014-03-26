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
#include <QMutex>

#include <ogre_engine.h>
#include <camera_grabber.h>
#include <camera.h>
#include <config.h>
#include <common.h>
#include <track_model.h>
#include <gui_controller.h>
#include <property_model.h>

using namespace track_library;

class GuiController;

class GlobalRenderer : public QQmlApplicationEngine
{
    Q_OBJECT

public:

    explicit GlobalRenderer(QWindow *parent = 0);
    ~GlobalRenderer();

    OgreEngine * getOgreEngine();
    Ogre::SceneManager * getSceneManager();
    QQuickWindow * getQuickWindow(){return m_root_window;}
    GuiController * getGuiController(){return m_gui_controller;}

signals:
    void startSimulation();
    void ogreInitialized();
    void queryExit();
    void contextObjectsSet();

public slots:
    void initializeOgre();

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
    bool m_closing;
    QQuickWindow * m_root_window;
    GuiController * m_gui_controller;
};

#endif
