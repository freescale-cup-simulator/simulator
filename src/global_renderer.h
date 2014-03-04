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

using namespace track_library;

class GuiController;

class GlobalRenderer : public QQmlApplicationEngine
{
    Q_OBJECT

public:

    explicit GlobalRenderer(QWindow *parent = 0);

    inline OgreEngine * ogreEngine(){return m_ogre_engine;}
    inline QQuickWindow * rootWindow(){return m_root_window;}

signals:
    void ogreInitialized();
    void queryExit();
    void contextObjectsSet();

public slots:
    void initializeOgre();

private slots:
    void setContextObjects();

private:

    OgreEngine * m_ogre_engine;    
    QQuickWindow * m_root_window;
    GuiController * m_gui_controller;
};

#endif
