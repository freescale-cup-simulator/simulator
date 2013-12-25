#include <camera_simulator.h>

CameraSimulator::CameraSimulator(Ogre::Camera * camera,SharedImage * buffer,QObject *parent)
    : QObject(parent)
    , m_camera(camera)
    , m_buffer(buffer)
{
}

CameraSimulator::~CameraSimulator()
{
}

void CameraSimulator::process(DataSet & data)
{
    Q_ASSERT(m_camera);
    Q_ASSERT(m_buffer);

    m_camera->setPosition(data.camera_position.x(), data.camera_position.y(),
                          data.camera_position.z());

    Ogre::Quaternion q(data.camera_rotation.scalar(), data.camera_rotation.x(),
                       data.camera_rotation.y(), data.camera_rotation.z());
    Ogre::Quaternion ry(Ogre::Radian(M_PI), Ogre::Vector3::UNIT_Y);
    Ogre::Quaternion rx(Ogre::Radian(-M_PI_4), Ogre::Vector3::UNIT_X);

    m_camera->setOrientation(q*ry*rx);

    quint8 * imageData = m_buffer->lock();
    QImage qimg(imageData, m_buffer->size().width(),m_buffer->size().height(),
                QImage::Format_ARGB32);
    QImage scaled=qimg.scaledToWidth(128);

    QRgb * line = reinterpret_cast<QRgb *>(scaled.scanLine(scaled.height() / 2));

    for(int i=0;i<128;i++)
        data.camera_pixels[i] = qGray(line[i]);

    m_buffer->unlock();
}
