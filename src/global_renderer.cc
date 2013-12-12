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
    //camera->setPosition(-2,0,2);
    //camera->pitch(Ogre::Degree(0));
    //camera->yaw(Ogre::Degree(0));
    //camera->roll(Ogre::Degree(0));
    //Ogre::Quaternion rot_up(Ogre::Degree(90),Ogre::Vector3::UNIT_Y);
    //camera->rotate(rot_up);
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
    m_ogre_head->setPosition(data.camera_position.x(),
                             data.camera_position.y() + 1,
                             data.camera_position.z());

    Ogre::Quaternion q(data.camera_rotation_quat.scalar(),
                       data.camera_rotation_quat.x(),
                       data.camera_rotation_quat.y(),
                       data.camera_rotation_quat.z());

    Ogre::Quaternion qy(Ogre::Radian(M_PI), Ogre::Vector3::UNIT_Y);

    m_ogre_head->setOrientation(-q * qy);
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
    disconnect(this, &GlobalRenderer::beforeRendering, this, &GlobalRenderer::initializeOgre);
    m_ogre_engine=new OgreEngine(this);
    m_root=m_ogre_engine->startEngine(RESOURCE_DIRECTORY "plugins.cfg");

    Ogre::ResourceGroupManager & rm = Ogre::ResourceGroupManager::getSingleton();
    rm.addResourceLocation( RESOURCE_DIRECTORY "materials", "FileSystem");
    rm.addResourceLocation( RESOURCE_DIRECTORY "meshes", "FileSystem");
    rm.addResourceLocation( RESOURCE_DIRECTORY "data.zip", "Zip");
    rm.initialiseAllResourceGroups();

    m_scene_manager = m_root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");

    Ogre::Camera * camera = m_scene_manager->createCamera("user_camera");
    camera->setNearClipDistance(0.1);
    camera->setFarClipDistance(99999);
    camera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
    camera->setPosition(0, 5, 0);
    camera->setOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90)), Ogre::Vector3::UNIT_X));
    //camera->lookAt(camera->getPosition().normalisedCopy());
    //camera->setFixedYawAxis(true,Ogre::Vector3::UNIT_Y);
    m_camera_controller=new OgreBites::SdkCameraMan(camera);
    m_camera_controller->setTopSpeed(10);
    m_user_camera=new Camera(camera,m_camera_controller);

    m_ogre_engine->activateOgreContext();
    m_scene_manager->setSkyBox(true, "SpaceSkyBox", 10000);

    m_scene_manager->createLight("light")->setPosition(0, 10, 0);

    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    auto gn = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
    Ogre::MeshManager::getSingleton().createPlane("ground", gn, plane,
                                                  m_track_model->width(),
                                                  m_track_model->height(),
                                                  m_track_model->width(),
                                                  m_track_model->height(),
                                                  false, 1, 5, 5,
                                                  Ogre::Vector3::UNIT_Z);

    Ogre::Entity* entGround = m_scene_manager->createEntity("GroundEntity",
                                                            "ground");
    auto groundNode=m_scene_manager->getRootSceneNode()->createChildSceneNode();

    groundNode->attachObject(entGround);
    entGround->setMaterialName("ground");

    groundNode->setPosition(m_track_model->width()/2, -0.01,
                            m_track_model->height()/2);

    Ogre::Entity * ogreEnt=m_scene_manager->createEntity("Head", "ogrehead.mesh");
    m_ogre_head=m_scene_manager->getRootSceneNode()->createChildSceneNode();
    m_ogre_head->attachObject(ogreEnt);
    m_ogre_head->setScale(0.01,0.01,0.01);
    //m_scene_manager->getRootSceneNode()->attachObject(ogreEnt);

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

        Ogre::SceneNode* node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
        node->attachObject(current);
        node->setPosition(-tile.x() + 0.5, 0, tile.y() + 0.5);
        node->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-tile.rotation() + 90));
        node->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90));
    }

    Ogre::Entity * current = m_scene_manager->createEntity("crossing.mesh");
    Ogre::SceneNode* node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(current);
    node->setPosition(0, 3, 0);
    node->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90));

    current = m_scene_manager->createEntity("line.mesh");
    node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(current);
    node->setPosition(5, 3, 0);
    node->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90));

    current = m_scene_manager->createEntity("turn.mesh");
    node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(current);
    node->setPosition(0, 3, 5);

    node->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90));
    m_ogre_engine->doneOgreContext();
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
    disconnect(this,&GlobalRenderer::statusChanged,this,&GlobalRenderer::onStatusChanged);
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

