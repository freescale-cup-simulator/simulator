#include <camera_simulator.h>

CameraSimulator::CameraSimulator(GlobalRenderer * renderer, QObject *parent)
    : QObject(parent)
    , m_engine(renderer->ogreEngine())
    , m_renderTarget(0)
    , m_camera(0)
    , m_manager(m_engine->sceneManager())
    , m_renderer(renderer)
    , m_camera_object(0)
{
    m_camera=m_manager->createCamera("CS_0");
    m_camera->setNearClipDistance(0.1);
    m_camera->setFarClipDistance(99999);
    m_camera->setAspectRatio(1);
    m_camera->setFOVy(Ogre::Radian(Ogre::Degree(110)));

    m_camera_object=new Camera(m_camera,this);

    m_frame=new quint8[CAMERA_FRAME_LEN];
    memset(m_frame, 0, CAMERA_FRAME_LEN);
    // Qt::DirectConnection will run CameraSimulator::onUpdate in QML Rendering Thread
    connect(renderer->rootWindow(), &QQuickWindow::beforeRendering, this,
            &CameraSimulator::onUpdate, Qt::DirectConnection);
    renderer->rootContext()->setContextProperty("carCamera", m_camera_object);
    emit created(m_camera_object);
}

CameraSimulator::~CameraSimulator()
{
    QMutexLocker locker(&m_safe_destruct);
    disconnect(m_renderer->rootWindow(), &QQuickWindow::beforeRendering, this,
               &CameraSimulator::onUpdate);
    emit destroyed();
    qDebug()<<"Camera destructor";
    delete[] m_frame;
    m_manager->destroyCamera(m_camera);
}

void CameraSimulator::process(DataSet & data)
{
    // this code is executed in SimulationRunner thread
    Q_ASSERT(m_camera);
    m_camera->setPosition(data.camera.position);

    Ogre::Quaternion ry(Ogre::Radian(M_PI), Ogre::Vector3::UNIT_Y);
    Ogre::Quaternion rx(Ogre::Radian(Ogre::Degree(-45)), Ogre::Vector3::UNIT_X);

    m_camera->setOrientation(data.camera.rotation * ry * rx);
    m_frame_locker.lock();
    memcpy(data.camera_pixels,m_frame,CAMERA_FRAME_LEN);
    m_frame_locker.unlock();
}

void CameraSimulator::onUpdate()
{
    // this code is executed in QML Rendering Thread

    Q_ASSERT(m_engine);
    if(!m_safe_destruct.tryLock())
        return;
    m_engine->activateOgreContext();
    /*Rendering code*/
    if (m_renderTarget)
        Ogre::TextureManager::getSingleton().remove(m_camera->getName());
    Ogre::TexturePtr m_rttTexture;
    int samples = m_engine->ogreContext()->format().samples();
    m_rttTexture = Ogre::TextureManager::getSingleton()
            .createManual(
                m_camera->getName(),
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::TEX_TYPE_2D,
                CAMERA_FRAME_LEN,
                CAMERA_FRAME_LEN,
                0,
                Ogre::PF_R8G8B8A8,
                    Ogre::TU_RENDERTARGET, 0, false,
                samples
                );

    m_renderTarget = m_rttTexture->getBuffer()->getRenderTarget();
    m_renderTarget->addViewport(m_camera);
    m_renderTarget->getViewport(0)->setClearEveryFrame(true);
    m_renderTarget->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
    m_renderTarget->getViewport(0)->setOverlaysEnabled(false);
    Ogre::Real aspectRatio = 1;
    m_camera->setAspectRatio(aspectRatio);
    m_renderTarget->update(true);
    // Hardware Buffer for the texture
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_rttTexture->getBuffer();
    Ogre::Image::Box lockBox(0,0,CAMERA_FRAME_LEN,CAMERA_FRAME_LEN);
    pixelBuffer->lock(lockBox,Ogre::HardwareBuffer::HBL_NORMAL);
    const Ogre::PixelBox &pixBox  = pixelBuffer->getCurrentLock();
    Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixBox.data);
    QImage qimg(pDest, CAMERA_FRAME_LEN,CAMERA_FRAME_LEN,
                QImage::Format_ARGB32);
    //qimg.save("file.bmp");
    QRgb * line = reinterpret_cast<QRgb *>(qimg.scanLine(CAMERA_FRAME_LEN / 2));
    m_frame_locker.lock();
    for(int i=0;i<CAMERA_FRAME_LEN;i++)
        m_frame[i] = qGray(line[i]);
    m_frame_locker.unlock();
    pixelBuffer->unlock();
    m_engine->doneOgreContext();
    m_safe_destruct.unlock();
    /*END rendering code*/
}
