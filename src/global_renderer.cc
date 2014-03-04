#include <global_renderer.h>

GlobalRenderer::GlobalRenderer(QWindow *parent)
    : QQmlApplicationEngine(parent)
    , m_ogre_engine(0)
    , m_root_window(nullptr)
    , m_gui_controller(new GuiController(this))
{
    qmlRegisterType<CameraGrabber>("OgreTypes", 1, 0, "CameraGrabber");
    qmlRegisterType<GlobalRenderer>("OgreTypes", 1, 0, "GlobalRenderer");
    qmlRegisterType<OgreEngine>("OgreTypes", 1, 0, "OgreEngine");
    qmlRegisterType<GuiController>("OgreTypes", 1, 0, "GuiController");
    addImportPath(":/qml/gui/");
    addImportPath(":/qml/");
    rootContext()->setContextProperty("globalRenderer", this);
    rootContext()->setContextProperty("guiController", m_gui_controller);
    load(QUrl("qrc:/qml/gui.qml"));

    m_root_window = qobject_cast<QQuickWindow *>(rootObjects().at(0));
    Q_ASSERT(m_root_window->objectName() == "rootWindow");

    connect(m_root_window, &QQuickWindow::beforeRendering, this, &GlobalRenderer::initializeOgre, Qt::DirectConnection);
    connect(this, &GlobalRenderer::ogreInitialized, this, &GlobalRenderer::setContextObjects);
    //connect(this, &GlobalRenderer::beforeRendering, this, &GlobalRenderer::initializeOgre, Qt::DirectConnection);
    //connect(this, &GlobalRenderer::ogreInitialized, this, &GlobalRenderer::addContent);
    //connect(this,&GlobalRenderer::statusChanged,this,&GlobalRenderer::onStatusChanged);

}

void GlobalRenderer::setContextObjects()
{
    rootContext()->setContextProperty("rootWindow", m_root_window);
    rootContext()->setContextProperty("ogreEngineInstance", m_ogre_engine);
    qDebug() << "done";
    emit contextObjectsSet();
}

void GlobalRenderer::initializeOgre()
{
    disconnect(m_root_window, &QQuickWindow::beforeRendering, this, &GlobalRenderer::initializeOgre);
    m_ogre_engine=new OgreEngine(m_root_window);
    m_ogre_engine->startEngine(RESOURCE_DIRECTORY "plugins.cfg");
    m_gui_controller->setOgreEngine(m_ogre_engine);
    /*m_ogre_engine->activateOgreContext();
    Ogre::ResourceGroupManager & rm = Ogre::ResourceGroupManager::getSingleton();
    rm.addResourceLocation(RESOURCE_DIRECTORY "materials", "FileSystem");
    rm.addResourceLocation(RESOURCE_DIRECTORY "meshes", "FileSystem");
    rm.initialiseAllResourceGroups();
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(4);

    m_scene_manager = m_root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
    m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    m_scene_manager->setAmbientLight(Ogre::ColourValue(255, 255, 255));
    Ogre::Camera * camera = m_scene_manager->createCamera("user_camera");
    camera->setNearClipDistance(1e-3);
    camera->setFarClipDistance(1e3);
    camera->setAspectRatio(Ogre::Real(m_root_window->width()) / Ogre::Real(m_root_window->height()));
    camera->setPosition(0, 5, 0);
    camera->setOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90)),
                                            Ogre::Vector3::UNIT_X));
    m_camera_controller=new OgreBites::SdkCameraMan(camera);
    m_camera_controller->setTopSpeed(10);
    //m_camera_controller->setStyle(OgreBites::CS_ORBIT);
    //m_camera_controller->setYawPitchDist(Ogre::Radian(Ogre::Degree(45)),Ogre::Radian(Ogre::Degree(45)),5);
    m_user_camera=new Camera(camera,m_camera_controller);

    m_ogre_engine->doneOgreContext();*/
    emit ogreInitialized();
}

