#include <global_renderer.h>

GlobalRenderer::GlobalRenderer(TrackModel *model, QWindow *parent)
    : QQuickView(parent)
    , m_track_model(model)
    , m_user_camera(0)
    , m_camera_controller(0)
    , m_ogre_engine(0)
    , m_root(0)
{
    connect(this, &GlobalRenderer::beforeRendering, this, &GlobalRenderer::initializeOgre, Qt::DirectConnection);
    connect(this, &GlobalRenderer::ogreInitialized, this, &GlobalRenderer::addContent);
    connect(this,&GlobalRenderer::statusChanged,this,&GlobalRenderer::onStatusChanged);
    connect(this,&GlobalRenderer::afterRendering,this,&GlobalRenderer::onFrameSwapped);
    qmlRegisterType<CameraGrabber>("CameraGrabber", 1, 0, "CameraGrabber");
    qmlRegisterType<SharedImage>("SharedImage",1,0,"SharedImage");
}

GlobalRenderer::~GlobalRenderer()
{
    if(m_scene_manager)
        m_root->destroySceneManager(m_scene_manager);
}

SharedImage *GlobalRenderer::createCameraGrabber()
{
    QString camera_name="CS_";
    camera_name.append(QString::number(m_shared_images.size()));
    Ogre::Camera * camera=m_scene_manager->createCamera(camera_name.toStdString().c_str());
    camera->setNearClipDistance(1);
    camera->setFarClipDistance(99999);
    camera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
    camera->setPosition(0,0,2);
    SharedImage * buffer=new SharedImage();
    m_shared_images<<buffer;
    Camera * cameraObject=new Camera(camera);
    CameraGrabber * grabber=new CameraGrabber(m_ogre_engine,cameraObject,buffer);
    //grabber->setOgreEngine(m_ogre_engine);
    //grabber->setCamera(cameraObject);

    qDebug()<<this->rootObject()->objectName();
    grabber->setParentItem(this->rootObject());
    grabber->setWidth(grabber->parentItem()->width());
    grabber->setHeight(grabber->parentItem()->height());
    grabber->setPosition(QPointF(-grabber->parentItem()->width(),0));
    return buffer;
}

void GlobalRenderer::initializeOgre()
{
    disconnect(this, &GlobalRenderer::beforeRendering, this, &GlobalRenderer::initializeOgre);
    m_ogre_engine=new OgreEngine(this);
    m_root=m_ogre_engine->startEngine(RESOURCE_DIRECTORY "plugins.cfg");

    Ogre::ResourceGroupManager::getSingleton().addResourceLocation( RESOURCE_DIRECTORY "materials", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation( RESOURCE_DIRECTORY "meshes", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation( RESOURCE_DIRECTORY "data.zip", "Zip");
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_scene_manager = m_root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");

    Ogre::Camera * camera = m_scene_manager->createCamera("user_camera");
    camera->setNearClipDistance(1);
    camera->setFarClipDistance(99999);
    camera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
    camera->setPosition(0,0,2);
    m_camera_controller=new OgreBites::SdkCameraMan(camera);
    m_user_camera=new Camera(camera,m_camera_controller);

    m_ogre_engine->activateOgreContext();
    m_scene_manager->setSkyBox(true, "SpaceSkyBox", 10000);

    m_scene_manager->createLight("light")->setPosition(0, 0, 10);

    Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0);
    Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                  plane, m_track_model->width(), m_track_model->height(), m_track_model->width(), m_track_model->height(), true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

    Ogre::Entity* entGround = m_scene_manager->createEntity("GroundEntity", "ground");
    auto groundNode=m_scene_manager->getRootSceneNode()->createChildSceneNode();


    groundNode->attachObject(entGround);
    entGround->setMaterialName("ground");

    groundNode->setPosition(m_track_model->width()/2,m_track_model->height()/2,0);

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
        node->setPosition(tile.x()-0.5,tile.y()+0.5,0);
        node->rotate(Ogre::Vector3::UNIT_Z,Ogre::Degree(180-tile.rotation()));
    }

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
    if(status==QQuickView::Ready)
        m_img=createCameraGrabber();
}

void GlobalRenderer::onFrameSwapped()
{
    //disconnect(this,&GlobalRenderer::afterRendering,this,&GlobalRenderer::onFrameSwapped);
    //qDebug()<<"Frame rendered "<<m_img->size();
    //m_img->waitNoEmpty();
    quint8 * data=m_img->lock();
    QImage img (data, 800,600,QImage::Format_ARGB32);
    img.save("screenshot.bmp");
    m_img->unlock();
    emit startSimulation();
}
