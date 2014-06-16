#include <linescan_camera.h>

LineScanCamera::LineScanCamera(Ogre::Camera *camera, QObject *parent)
    : Camera(camera, parent)
{
    m_camera->setNearClipDistance(0.1);
    m_camera->setFarClipDistance(99999);
    m_camera->setAspectRatio(1);
    m_camera->setFOVy(Ogre::Radian(Ogre::Degree(110)));

    memset(m_current_frame, 0, CAMERA_FRAME_LEN);
}

LineScanCamera::~LineScanCamera()
{
}

void LineScanCamera::process(QPair<Ogre::Vector3, Ogre::Quaternion> state, quint8 line[])
{
    m_camera->setPosition(state.first);

    Ogre::Quaternion ry(Ogre::Radian(M_PI), Ogre::Vector3::UNIT_Y);
    Ogre::Quaternion rx(Ogre::Radian(Ogre::Degree(-45)), Ogre::Vector3::UNIT_X);
    m_camera->setOrientation(state.second * ry * rx);

    m_frame_locker.lock();
    memcpy(line, m_current_frame, CAMERA_FRAME_LEN);
    m_frame_locker.unlock();
}

void LineScanCamera::update()
{
    Q_ASSERT(m_rendering_objects);

    m_rendering_objects->engine->activateOgreContext();

    if (m_texture.isNull())
        setup();

    m_texture->getBuffer()->getRenderTarget()->update(true);

    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_texture->getBuffer();
    Ogre::Image::Box lockBox(0, 0, CAMERA_FRAME_LEN, CAMERA_FRAME_LEN);

    pixelBuffer->lock(lockBox, Ogre::HardwareBuffer::HBL_NORMAL);

    const Ogre::PixelBox &pixBox = pixelBuffer->getCurrentLock();
    Ogre::uint8 * data = static_cast<Ogre::uint8 *>(pixBox.data);

    QImage image(data, CAMERA_FRAME_LEN, CAMERA_FRAME_LEN, QImage::Format_ARGB32);
    QRgb * line = reinterpret_cast<QRgb *>(image.scanLine(CAMERA_FRAME_LEN / 2));
//    qimg.save("file.bmp");

    m_frame_locker.lock();
    for (int i = 0; i < CAMERA_FRAME_LEN; i++)
        m_current_frame[i] = qGray(line[i]);
    m_frame_locker.unlock();

    pixelBuffer->unlock();
    m_rendering_objects->engine->doneOgreContext();
}

void LineScanCamera::setup()
{
    Ogre::TextureManager & m = Ogre::TextureManager::getSingleton();
    int samples = m_rendering_objects->window->format().samples();
    auto gn = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

    m_texture = m.createManual(m_camera->getName(), gn, Ogre::TEX_TYPE_2D,
                               CAMERA_FRAME_LEN, CAMERA_FRAME_LEN,
                               0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET,
                               0, false, samples);

    auto rt = m_texture->getBuffer()->getRenderTarget();
    rt->addViewport(m_camera);
    rt->getViewport(0)->setClearEveryFrame(true);
    rt->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
    rt->getViewport(0)->setOverlaysEnabled(false);
}
