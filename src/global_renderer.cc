#include <global_renderer.h>

GlobalRenderer::GlobalRenderer(QWindow *parent)
    : QQmlApplicationEngine(parent)
    , m_ogre_engine(nullptr)
    , m_root_window(nullptr)
    , m_user_camera(nullptr)
{

    addImportPath(":/qml/gui/");
    addImportPath(":/qml/");
    rootContext()->setContextProperty("globalRenderer", this);
}

void GlobalRenderer::create()
{
    load(QUrl("qrc:/qml/gui.qml"));

    m_root_window = qobject_cast<QQuickWindow *>(rootObjects().at(0));
    Q_ASSERT(m_root_window->objectName() == "rootWindow");

    connect(m_root_window, &QQuickWindow::beforeRendering, this,
            &GlobalRenderer::initializeOgre, Qt::DirectConnection);
    connect(this, &GlobalRenderer::ogreInitialized, this,
            &GlobalRenderer::setContextObjects);

    QEventLoop loop;
    connect (this, &GlobalRenderer::contextObjectsSet, &loop, &QEventLoop::quit);
    loop.exec();
}

void GlobalRenderer::setContextObjects()
{
    rootContext()->setContextProperty("rootWindow", m_root_window);
    rootContext()->setContextProperty("ogreEngineInstance", m_ogre_engine);
    rootContext()->setContextProperty("userCamera",m_user_camera);
    qDebug() << "done";
    emit contextObjectsSet();
}

void GlobalRenderer::initializeOgre()
{
    disconnect(m_root_window, &QQuickWindow::beforeRendering, this, &GlobalRenderer::initializeOgre);
    m_ogre_engine=new OgreEngine(m_root_window);
    m_ogre_engine->startEngine(RESOURCE_DIRECTORY,RESOURCE_DIRECTORY "plugins.cfg");
    Ogre::SceneManager * scene_manager=m_ogre_engine->sceneManager();
    Ogre::Camera * camera = scene_manager->createCamera("user_camera");
    camera->setNearClipDistance(1e-3);
    camera->setFarClipDistance(1e3);
    camera->setAspectRatio(Ogre::Real(m_root_window->width()) / Ogre::Real(m_root_window->height()));
    camera->setPosition(0, 5, 0);
    camera->setOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90)),
                                            Ogre::Vector3::UNIT_X));
    OgreBites::SdkCameraMan * camera_controller=new OgreBites::SdkCameraMan(camera);
    camera_controller->setTopSpeed(10);
    m_user_camera=new Camera(camera,camera_controller);
    emit ogreInitialized();
}

