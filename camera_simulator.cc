#include <camera_simulator.h>

CameraSimulator::CameraSimulator(Ogre::Camera * camera,SharedImage * buffer,QObject *parent)
    : QObject(parent)
    , m_camera(camera)
    , m_buffer(buffer)
{
    m_step=0;
    m_initial_orientation=m_camera->getOrientation();
    //m_camera->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);

}

CameraSimulator::~CameraSimulator()
{
}

/*void CameraSimulator::init()
{
    SIrrlichtCreationParameters p;
    p.AntiAlias = 8;
    p.DriverType = video::EDT_OPENGL;
    p.WindowSize = m_device_size;
    p.LoggingLevel = ELL_INFORMATION;

    m_device = createDeviceEx(p);
    Q_ASSERT(m_device);

    m_scene_manager = m_device->getSceneManager();
    m_video_driver = m_device->getVideoDriver();
    m_camera = m_scene_manager->addCameraSceneNode();
    m_camera->setUpVector(core::vector3df(0,1,0));

#if 1
    int w = m_track_model.width() * 2;
    int h = m_track_model.height() * 2;
    auto pm = m_scene_manager->addHillPlaneMesh("ground", core::dimension2df(w, h),
                                          core::dimension2du(1, 1));
    auto plane = m_scene_manager->addAnimatedMeshSceneNode(pm);
    plane->setMaterialTexture(0,m_video_driver->getTexture(RESOURCE_DIRECTORY
                                                     "images/texture.jpg"));
    plane->setMaterialFlag(video::EMF_LIGHTING, false);
    plane->setPosition(core::vector3df(m_track_model.width() / 2, -0.1,
                                       m_track_model.height() / 2));
#endif

    QString modelName;
    QString textureName;
    for (const Tile & tile : m_track_model.tiles())
    {
        switch(tile.type())
        {
        case Tile::Line:
            modelName = RESOURCE_DIRECTORY "models/line.dae";
            textureName = RESOURCE_DIRECTORY "images/line.jpg";
            break;
        case Tile::Crossing:
            modelName = RESOURCE_DIRECTORY "models/crossing.dae";
            textureName = RESOURCE_DIRECTORY "images/crossing.png";
            break;
        case Tile::Start:
            modelName = RESOURCE_DIRECTORY "models/start.dae";
            textureName = RESOURCE_DIRECTORY "images/start.png";
            break;
        case Tile::Saw:
            modelName = RESOURCE_DIRECTORY "models/saw.dae";
            textureName = RESOURCE_DIRECTORY "images/saw.png";
            break;
        case Tile::Turn:
            modelName = RESOURCE_DIRECTORY "models/turn.dae";
            textureName = RESOURCE_DIRECTORY "images/turn.png";
            break;
        default:
            modelName = RESOURCE_DIRECTORY "models/empty.dae";
            textureName = RESOURCE_DIRECTORY "images/empty.png";
        }

        auto tileMesh = m_scene_manager->getMesh(modelName.toStdString().c_str());
        auto tilePlane = m_scene_manager->addAnimatedMeshSceneNode(tileMesh);

        tilePlane->setScale(core::vector3df(0.5,0,0.5));
        tilePlane->setMaterialFlag(video::EMF_LIGHTING, false);
        tilePlane->setRotation(core::vector3df(0, tile.rotation(), 0));
        tilePlane->setPosition(core::vector3df(tile.x() + 0.5, 0,
                                               tile.y() + 0.5));
    }

    m_target=m_scene_manager->addBillboardSceneNode(0,
                                              core::dimension2df(0.1,0.1),
                                              core::vector3df(0,0,0));
    m_target->setMaterialFlag(video::EMF_LIGHTING, false);
    m_target->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
    m_target->setMaterialTexture(0,m_video_driver->getTexture(RESOURCE_DIRECTORY
                                                        "images/target.png"));
    m_frame_texture =
            m_video_driver->addRenderTargetTexture(m_device_size, "frame",
                                                   video::ECF_R8G8B8);
    m_camera->bindTargetAndRotation(true);
}*/

void CameraSimulator::process(DataSet & data)
{
    Q_ASSERT(m_camera);
    Q_ASSERT(m_buffer);
    //qDebug()<<"Position: "<<data.camera_position.x()<<","<<data.camera_position.y()<<","<<data.camera_position.z();
    m_camera->setPosition(data.camera_position.x(),data.camera_position.y(),data.camera_position.z());

    //m_camera->setPosition(); ??????
    //m_camera->move(Ogre::Vector3(data.camera_position.x(),data.camera_position.y(),data.camera_position.z()+0.5));
    /*m_camera->setPosition(core::vector3df(data.camera_position.x(),
                                          data.camera_position.z(),
                                          data.camera_position.y()));*/

    //Ogre::Quaternion rot_x(Ogre::Degree(data.camera_rotation.x()),Ogre::Vector3::UNIT_X);
    //Ogre::Quaternion rot_y(Ogre::Degree(data.camera_rotation.y()),Ogre::Vector3::UNIT_Y);
    //Ogre::Quaternion rot_z(Ogre::Degree(data.camera_rotation.z()),Ogre::Vector3::UNIT_Z);
    //Ogre::Quaternion rot_up(Ogre::Degree(90),Ogre::Vector3::UNIT_Y);
    //m_camera->rotate(rot_x*rot_y*rot_z);
    //m_camera->roll(Ogre::Degree(data.camera_rotation.z()));
    //m_camera->yaw(Ogre::Degree(data.camera_rotation.y()));
    //m_camera->pitch(Ogre::Degree(data.camera_rotation.x()));
    //Ogre::Vector3 vect=m_camera->getDirection().normalisedCopy();
    //qDebug()<<"Direction: "<<vect.x<<","<<vect.y<<","<<vect.z;

    //qDebug()<<"Cam rot. X: "<<data.camera_rotation.x();
    //qDebug()<<"Cam rot. Z: "<<data.camera_rotation.z();

    //m_camera->rotate(Ogre::Vector3::UNIT_X,Ogre::Degree(0));
    //m_camera->rotate(Ogre::Vector3::UNIT_X,Ogre::Degree(data.camera_rotation.x()));
    //m_camera->rotate(Ogre::Vector3::UNIT_Z,Ogre::Radian(data.camera_rotation.z()));
    //m_camera->rotate(Ogre::Vector3::UNIT_Y,Ogre::Radian(data.camera_rotation.y()));
    //m_camera->roll(Ogre::Degree(data.camera_rotation.z()));
    //m_camera->pitch(Ogre::Radian(data.camera_rotation.x()));

    //m_camera->yaw(Ogre::Degree(data.camera_rotation.z()));
    Ogre::Quaternion q(data.camera_rotation_quat.scalar(),data.camera_rotation_quat.x(),data.camera_rotation_quat.y(),data.camera_rotation_quat.z());
    Ogre::Quaternion q2(Ogre::Radian(0.95),Ogre::Vector3::UNIT_X);
    m_camera->setOrientation(q*q2);
    //qDebug()<<"RotX:"<<data.camera_rotation.x()<<"; RotY:"<<data.camera_rotation.y()<<"; RotZ: "<<data.camera_rotation.z();
    //m_camera->rotate(rot_x*rot_y*m_initial_orientation);
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
    {
        data.camera_pixels[i] = qGray(line[i]);
        //grayscale.setPixel(i,0,qRgb(data.camera_pixels[i],data.camera_pixels[i],data.camera_pixels[i]));
    }
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
