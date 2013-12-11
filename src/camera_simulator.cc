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
    //qDebug()<<"Position: "<<data.camera_position.x()<<","<<data.camera_position.y()<<","<<data.camera_position.z();
    m_camera->setPosition(data.camera_position.x(),data.camera_position.y(),data.camera_position.z());
    Ogre::Quaternion q(data.camera_rotation_quat.scalar(),data.camera_rotation_quat.x(),data.camera_rotation_quat.y(),data.camera_rotation_quat.z());
    Ogre::Quaternion q2(Ogre::Radian(0.95),Ogre::Vector3::UNIT_X);
    m_camera->setOrientation(q*q2);
    quint8 * imageData = m_buffer->lock();
    QImage qimg(imageData, m_buffer->size().width(),m_buffer->size().height(), QImage::Format_ARGB32);
    QImage scaled=qimg.scaledToWidth(128);
    //qimg.save("qimg.bmp");
    //scaled.save("scaled.bmp");
    //qDebug()<<qimg.size();
    //Q_ASSERT(0);
    QRgb * line = reinterpret_cast<QRgb *>(scaled.scanLine(scaled.height() / 2));
    //quint32 startPos=(m_buffer->size().width()-CAMERA_FRAME_LEN)/2;
    //quint32 index=0;
    //QImage grayscale(128,1,QImage::Format_RGB888);
    for(int i=0;i<128;i++)
        data.camera_pixels[i] = qGray(line[i]);
        //grayscale.setPixel(i,0,qRgb(data.camera_pixels[i],data.camera_pixels[i],data.camera_pixels[i]));
    /*while(index<128)
    {
        data.camera_pixels[index] = qGray(line[startPos+index]);
        grayscale.setPixel(index,0,qRgb(data.camera_pixels[index],data.camera_pixels[index],data.camera_pixels[index]));
        index++;
    }*/
    //grayscale.save("grayscale.bmp");
    //QByteArray test_array((char *)data.camera_pixels,128);
    //qDebug()<<test_array;
    //Q_ASSERT(m_step!=BREAK_ON_STEP);
    //m_step++;
    m_buffer->unlock();
}
