#ifndef GLOBAL_RENDERER_H
#define GLOBAL_RENDERER_H

#include <QCoreApplication>
#include <QtQml/QQmlContext>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QEventLoop>

#include <ogre_engine.h>
#include <camera.h>
#include <config.h>

// Loads GUI and OgreEngine
// Sets QML context property ogreEngineInstance
class GlobalRenderer : public QQmlApplicationEngine
{
    Q_OBJECT

public:

    explicit GlobalRenderer(QWindow *parent = 0);

    void create();

    inline OgreEngine * ogreEngine(){return m_ogre_engine;}
    inline QQuickWindow * rootWindow(){return m_root_window;}
    inline Camera * userCamera() { return m_user_camera; }

signals:
    void ogreInitialized();
    void contextObjectsSet();

public slots:
    void initializeOgre();

private slots:
    void setContextObjects();

private:
    OgreEngine * m_ogre_engine;
    QQuickWindow * m_root_window;
    Camera * m_user_camera;
};

class RenderingObjects
{
public:
    RenderingObjects(GlobalRenderer * renderer)
    {
        engine = renderer->ogreEngine();
        gl_context = new QOpenGLContext();
        gl_context->setShareContext(engine->ogreContext());

        window = renderer->rootWindow();
        scene_manager = engine->sceneManager();
    }

    OgreEngine * engine;
    QOpenGLContext * gl_context;
    QQuickWindow * window;
    Ogre::SceneManager * scene_manager;
};

class RenderingObjectsUser
{
public:
    virtual inline void setRenderingObjects(RenderingObjects * ro)
    {
        m_rendering_objects = ro;
    }

protected:
    RenderingObjects * m_rendering_objects;
};

#endif
