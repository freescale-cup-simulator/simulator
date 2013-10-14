#include <camera_simulator.h>

CameraSimulator::CameraSimulator(const TrackModel & track,
                                 QSize deviceSize,
                                 QObject *parent)
    : QObject(parent)
    , m_device_size(deviceSize.width(), deviceSize.height())
    , m_track_model(track)
{
    init();
}

core::vector3df CameraSimulator::getTargetPosition()
{
    return m_camera->getTarget();
}

CameraSimulator::~CameraSimulator()
{
    m_device->drop();
}

void CameraSimulator::init()
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
}

void CameraSimulator::process(DataSet & data)
{
    m_camera->setPosition(core::vector3df(data.camera_position.x(),
                                          data.camera_position.z(),
                                          data.camera_position.y()));

    m_camera->setRotation(core::vector3df(data.camera_rotation.x(),
                                          data.camera_rotation.y(),
                                          data.camera_rotation.z()));
    m_camera->updateAbsolutePosition();
    m_target->setPosition(m_camera->getTarget());
    m_target->updateAbsolutePosition();

    m_device->getTimer()->tick();
    m_device->getVideoDriver()->beginScene();
    m_video_driver->setRenderTarget(m_frame_texture);

    m_target->setVisible(false);
    m_device->getSceneManager()->drawAll();
    m_video_driver->setRenderTarget(0);
    m_target->setVisible(true);

    void * imageData = m_frame_texture->lock();
    QByteArray frame;
    QImage qimg(static_cast<unsigned char *>(imageData), m_device_size.Width,
                m_device_size.Height, QImage::Format_RGB888);
    QRgb * line = reinterpret_cast<QRgb *>(qimg.scanLine(m_device_size.Height / 2));

    for(unsigned int i = 0; i < m_device_size.Width; i++)
        data.camera_pixels[i] = qGray(line[i]);

    m_frame_texture->unlock();
    m_device->getSceneManager()->drawAll();
    m_device->getVideoDriver()->endScene();
}
