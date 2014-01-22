#include <global_renderer.h>

GlobalRenderer::GlobalRenderer(QWindow *parent)
    : QQuickView(parent)
    , m_user_camera(0)
    , m_camera_controller(0)
    , m_ogre_engine(0)
    , m_root(0)
    , m_track_model(0)
    , m_closing(false)
{
    connect(this, &GlobalRenderer::beforeRendering, this, &GlobalRenderer::initializeOgre, Qt::DirectConnection);
    connect(this, &GlobalRenderer::ogreInitialized, this, &GlobalRenderer::addContent);
    connect(this,&GlobalRenderer::statusChanged,this,&GlobalRenderer::onStatusChanged);
    //connect(this,&GlobalRenderer::afterRendering,this,&GlobalRenderer::onFrameSwapped);
    qmlRegisterType<CameraGrabber>("CameraGrabber", 1, 0, "CameraGrabber");
    //qmlRegisterType<Camera>("Camera", 1, 0, "Camera");
    //qmlRegisterType<SharedImage>("SharedImage",1,0,"SharedImage");
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

CameraGrabber *GlobalRenderer::createCameraGrabber(QSemaphore *sync)
{
    QString camera_name="CS_";
    camera_name.append(QString::number(m_camera_grabbers.size()));
    Ogre::Camera * camera=m_scene_manager->createCamera(camera_name.toStdString().c_str());
    camera->setNearClipDistance(0.1);
    camera->setFarClipDistance(99999);
    camera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
    SharedImage * buffer=new SharedImage(sync);
    Camera * cameraObject=new Camera(camera);
    CameraGrabber * grabber=new CameraGrabber(m_ogre_engine,cameraObject,buffer);
    m_camera_grabbers<< grabber;
    grabber->setParentItem(this->rootObject());
    grabber->setWidth(grabber->parentItem()->width());
    grabber->setHeight(grabber->parentItem()->height());
    grabber->setPosition(QPointF(-grabber->parentItem()->width(),0));
    attachCamToGUI(0);
    return grabber;
}

void GlobalRenderer::process(DataSet &data)
{
    m_local_dataset_locker.tryLock();
    m_local_dataset=data;
    m_local_dataset_locker.unlock();
}

void GlobalRenderer::attachCamToGUI(quint32 index)
{
    CameraGrabber * grabber=m_camera_grabbers.at(index);
    Q_ASSERT(grabber);
    QObject * viewGrabber=this->rootObject()->findChild<QQuickItem *>("camViewContainer")->findChild<QObject *>("camView");
    Q_ASSERT(viewGrabber);
    qobject_cast<CameraGrabber *>(viewGrabber)->setCamera(grabber->camera());
}

void GlobalRenderer::initializeOgre()
{
    Q_ASSERT(m_track_model);
    disconnect(this, &GlobalRenderer::beforeRendering, this,
               &GlobalRenderer::initializeOgre);
    m_ogre_engine=new OgreEngine(this);
    m_root=m_ogre_engine->startEngine(RESOURCE_DIRECTORY "plugins.cfg");

    m_ogre_engine->activateOgreContext();
    Ogre::ResourceGroupManager & rm = Ogre::ResourceGroupManager::getSingleton();
    rm.addResourceLocation(RESOURCE_DIRECTORY "materials", "FileSystem");
    rm.addResourceLocation(RESOURCE_DIRECTORY "meshes", "FileSystem");
    rm.initialiseAllResourceGroups();

    m_scene_manager = m_root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
    m_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    m_scene_manager->setAmbientLight(Ogre::ColourValue(255, 255, 255));
    m_scene_manager->createLight("light")->setPosition(5, 10, 0);
    Ogre::Camera * camera = m_scene_manager->createCamera("user_camera");
    camera->setNearClipDistance(1e-3);
    camera->setFarClipDistance(1e3);
    camera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
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

    connect(this,&GlobalRenderer::beforeRendering,this,&GlobalRenderer::updateScene);

    emit ogreInitialized();
}

void GlobalRenderer::addContent()
{
    // expose objects as QML globals
    rootContext()->setContextProperty("Window", this);
    rootContext()->setContextProperty("Camera", m_user_camera);
    rootContext()->setContextProperty("OgreEngine", m_ogre_engine);

    // load the QML scene
    setResizeMode(QQuickView::SizeRootObjectToView);
    setSource(QUrl("qrc:/qml/ui.qml"));
}

void GlobalRenderer::onStatusChanged(QQuickView::Status status)
{
    disconnect(this, &GlobalRenderer::statusChanged, this,
               &GlobalRenderer::onStatusChanged);
    if(status==QQuickView::Ready)
        emit startSimulation();
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
    return QWindow::event(event);
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
