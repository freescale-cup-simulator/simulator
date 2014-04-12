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

    class RenderingInstances
    {
    public:
        RenderingInstances(GlobalRenderer * renderer)
        {
            OgreEngine * engine = renderer->m_ogre_engine;
            gl_context = new QOpenGLContext();
            gl_context->setShareContext(engine->ogreContext());

            surface = renderer->m_root_window;
            scene_manager = engine->sceneManager();
        }

        QOpenGLContext * gl_context;
        QSurface * surface;
        Ogre::SceneManager * scene_manager;
    };

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

#endif
