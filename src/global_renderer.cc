#include <global_renderer.h>

GlobalRenderer::GlobalRenderer(QWindow *parent)
    : QQmlApplicationEngine(parent)
    , m_user_camera(0)
    , m_camera_controller(0)
    , m_ogre_engine(0)
    , m_root(0)
    , m_track_model(0)
    , m_closing(false)
    , m_root_window(nullptr)
{
    qmlRegisterType<CameraGrabber>("OgreTypes", 1, 0, "CameraGrabber");
    qmlRegisterType<GlobalRenderer>("OgreTypes", 1, 0, "GlobalRenderer");
    qmlRegisterType<OgreEngine>("OgreTypes", 1, 0, "OgreEngine");

    addImportPath(":/qml/gui/");
    addImportPath(":/qml/");
    rootContext()->setContextProperty("globalRenderer", this);
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
    rootContext()->setContextProperty("userCamera", m_user_camera);
    rootContext()->setContextProperty("ogreEngineInstance", m_ogre_engine);
    qDebug() << "done";
    emit contextObjectsSet();
}

GlobalRenderer::~GlobalRenderer()
{
    if(m_scene_manager)
        m_root->destroySceneManager(m_scene_manager);
}

void GlobalRenderer::setTrackModel(TrackModel *model)
{
    m_track_model=model;
}


void GlobalRenderer::process(DataSet &data)
{
    m_local_dataset_locker.tryLock();
    m_local_dataset=data;
    m_local_dataset_locker.unlock();
}

OgreEngine *GlobalRenderer::getOgreEngine()
{
    return m_ogre_engine;
}

Ogre::SceneManager *GlobalRenderer::getSceneManager()
{
    return m_scene_manager;
}

void GlobalRenderer::initializeOgre()
{
    Q_ASSERT(m_track_model);
    disconnect(m_root_window, &QQuickWindow::beforeRendering, this, &GlobalRenderer::initializeOgre);
    m_ogre_engine=new OgreEngine(m_root_window);
    m_root=m_ogre_engine->startEngine(RESOURCE_DIRECTORY "plugins.cfg");

    m_ogre_engine->activateOgreContext();
    Ogre::ResourceGroupManager & rm = Ogre::ResourceGroupManager::getSingleton();
    rm.addResourceLocation(RESOURCE_DIRECTORY "materials", "FileSystem");
    rm.addResourceLocation(RESOURCE_DIRECTORY "meshes", "FileSystem");
    rm.initialiseAllResourceGroups();

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
    m_user_camera=new Camera(camera,m_camera_controller);
    Ogre::Entity * ent;

    ent = m_scene_manager->createEntity("car", "car_body.mesh");
    m_car_body = m_scene_manager->getRootSceneNode()->createChildSceneNode();
    m_car_body->attachObject(ent);

    for (int i = 0; i < 4; i++)
    {
        m_wheels[i] = m_scene_manager->getRootSceneNode()->createChildSceneNode();
        ent = m_scene_manager->createEntity("wheel_h.mesh");
        m_wheels[i]->attachObject(ent);
    }

    for (const Tile & tile : m_track_model->tiles())
    {
        Ogre::Entity* current=0;
        switch(tile.type())
        {
        case Tile::Line:
            current=m_scene_manager->createEntity("line.mesh");
            break;
        case Tile::Crossing:
            current=m_scene_manager->createEntity("crossing.mesh");
            break;
        case Tile::Start:
            current=m_scene_manager->createEntity("start.mesh");
            break;
        case Tile::Saw:
            current=m_scene_manager->createEntity("saw.mesh");
            break;
        case Tile::Turn:
            current=m_scene_manager->createEntity("turn.mesh");
            break;
        default:
            current=m_scene_manager->createEntity("crossing.mesh");
        }

        Ogre::SceneNode * node
                = m_scene_manager->getRootSceneNode()->createChildSceneNode();
        node->attachObject(current);
        node->setPosition(-tile.x() + 0.5, 0, tile.y() + 0.5);
        node->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-tile.rotation() - 180));
    }

    m_ogre_engine->doneOgreContext();

    connect(m_root_window, &QQuickWindow::beforeRendering,this,&GlobalRenderer::updateScene,Qt::DirectConnection);
    emit ogreInitialized();
}

void GlobalRenderer::onStatusChanged(QQuickView::Status status)
{
    /*disconnect(m_root_window, &QQuickWindow::statusChanged, this,
               &GlobalRenderer::onStatusChanged);
    if(status==QQuickView::Ready)
        emit startSimulation();*/
}

bool GlobalRenderer::event(QEvent *event)
{
    if(event->type()==QEvent::Close && !m_closing)
    {
        m_closing=true;
        emit queryExit();
        event->ignore();
        return false;
    }
    return QQmlEngine::event(event);
}

void GlobalRenderer::updateScene()
{
    m_local_dataset_locker.lock();
    m_ogre_engine->activateOgreContext();

    m_car_body->setPosition(m_local_dataset.vehicle.p.x(), m_local_dataset.vehicle.p.y(),
                            m_local_dataset.vehicle.p.z());
    Ogre::Quaternion q(m_local_dataset.vehicle.q.scalar(), m_local_dataset.vehicle.q.x(),
                       m_local_dataset.vehicle.q.y(), m_local_dataset.vehicle.q.z());
    m_car_body->setOrientation(q);
    m_car_body->_updateBounds();

    for (int i = 0; i < 4; i++)
    {
        m_wheels[i]->setPosition(m_local_dataset.wheels[i].p.x(), m_local_dataset.wheels[i].p.y(),
                                 m_local_dataset.wheels[i].p.z());
        Ogre::Quaternion q(m_local_dataset.wheels[i].q.scalar(), m_local_dataset.wheels[i].q.x(),
                           m_local_dataset.wheels[i].q.y(), m_local_dataset.wheels[i].q.z());
        m_wheels[i]->setOrientation(q);
        m_wheels[i]->_updateBounds();
    }
    m_ogre_engine->doneOgreContext();
    m_local_dataset_locker.unlock();
}

