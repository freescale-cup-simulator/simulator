#include <camera_simulator.h>

CameraSimulator::CameraSimulator(const TrackModel & track,
                                 QSize deviceSize,
                                 QObject *parent)
    : QObject(parent)
    , m_size(deviceSize)
    , m_track(track)
{
    init();
}

const QList<scene::IAnimatedMeshSceneNode *> *CameraSimulator::tiles3d()
{
    return &m_tiles3d;
}

core::vector3df CameraSimulator::getTargetPosition()
{
    return m_camera->getTarget();
}

CameraSimulator::~CameraSimulator()
{

}

void CameraSimulator::init()
{
    m_device = createDevice(video::EDT_OPENGL,
                            core::dimension2du(m_size.width(), m_size.height()));
    Q_ASSERT(m_device);

    m_manager=m_device->getSceneManager();
    m_driver=m_device->getVideoDriver();
    m_camera=m_manager->addCameraSceneNode();
    m_camera->setUpVector(core::vector3df(0,1,0));

//    int w = m_track.width() * 2;
//    int h = m_track.height() * 2;
//    auto pm = m_manager->addHillPlaneMesh("ground", core::dimension2df(w, h),
//                                          core::dimension2du(1, 1));
//    auto plane = m_manager->addAnimatedMeshSceneNode(pm);
//    plane->setMaterialTexture(0,m_driver->getTexture(RESOURCE_DIRECTORY
//                                                     "images/texture.jpg"));
//    plane->setMaterialFlag(video::EMF_LIGHTING, false);
//    plane->setPosition(core::vector3df(m_track.width() / 2, -0.1,
//                                       m_track.height() / 2));

    for (const Tile & tile : m_track.tiles())
    {
        QString modelName;
        QString textureName;
        switch(tile.type())
        {
        case Tile::Line:
            modelName=RESOURCE_DIRECTORY "models/line.dae";
            textureName=RESOURCE_DIRECTORY "images/line.jpg";
            break;
        case Tile::Crossing:
            modelName=RESOURCE_DIRECTORY "models/crossing.dae";
            textureName=RESOURCE_DIRECTORY "images/crossing.png";
            break;
        case Tile::Start:
            modelName=RESOURCE_DIRECTORY "models/start.dae";
            textureName=RESOURCE_DIRECTORY "images/start.png";
            break;
        case Tile::Saw:
            modelName=RESOURCE_DIRECTORY "models/saw.dae";
            textureName=RESOURCE_DIRECTORY "images/saw.png";
            break;
        case Tile::Turn:
            modelName=RESOURCE_DIRECTORY "models/turn.dae";
            textureName=RESOURCE_DIRECTORY "images/turn.png";
            break;
        default:
            modelName=RESOURCE_DIRECTORY "models/empty.dae";
            textureName=RESOURCE_DIRECTORY "images/empty.png";
        }

        auto tileMesh=m_manager->getMesh(modelName.toStdString().c_str());
        auto tilePlane=m_manager->addAnimatedMeshSceneNode(tileMesh);

        tilePlane->setScale(core::vector3df(0.5,0,0.5));
        tilePlane->setMaterialFlag(video::EMF_LIGHTING, false);
        tilePlane->setRotation(core::vector3df(0,tile.rotation(),0));
        tilePlane->setPosition(core::vector3df(tile.x() + 0.5, 0, tile.y() + 0.5));
        m_tiles3d<<tilePlane;
    }
    m_target=m_manager->addBillboardSceneNode(0,
                                              core::dimension2df(0.1,0.1),
                                              core::vector3df(0,0,0));
    m_target->setMaterialFlag(video::EMF_LIGHTING, false);
    m_target->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
    m_target->setMaterialTexture(0,m_driver->getTexture(RESOURCE_DIRECTORY
                                                        "images/target.png"));
    m_texture=m_driver->addRenderTargetTexture(core::dimension2du(m_size.width(),
                                                                  m_size.height()),
                                               "frame",video::ECF_R8G8B8);
    m_camera->bindTargetAndRotation(true);
}

void CameraSimulator::onUnload()
{
    m_device->drop();
    emit unloaded();
}

QByteArray CameraSimulator::onSimulatorResponse(const DataSet &params)
{
    Q_ASSERT(params.contains(CameraPositionX));
    Q_ASSERT(params.contains(CameraRotationX));

    m_camera->setPosition(core::vector3df(params[CameraPositionX].toFloat(),
                                          params[CameraPositionZ].toFloat(),
                                          params[CameraPositionY].toFloat()));

    m_camera->setRotation(core::vector3df(params[CameraRotationX].toFloat(),
                                          params[CameraRotationY].toFloat(),
                                          params[CameraRotationZ].toFloat()));
    m_camera->updateAbsolutePosition();
    m_target->setPosition(m_camera->getTarget());
    m_target->updateAbsolutePosition();

    m_device->getTimer()->tick();
    m_device->getVideoDriver()->beginScene(true, true);
    m_driver->setRenderTarget(m_texture, true, true);
    m_target->setVisible(false);
    m_device->getSceneManager()->drawAll();
    m_driver->setRenderTarget(0, true, true, 0);
    m_target->setVisible(true);
    void* imageData = m_texture->lock();
    m_frame.clear();
    QImage qimg((uchar*)imageData, m_size.width(),m_size.height(),QImage::Format_RGB888);
    QRgb * line=(QRgb *)qimg.scanLine(m_size.height()/2);
    for(int i=0;i<m_size.width();i++)
        m_frame.append(qGray(line[i]));
    m_texture->unlock();
    m_device->getSceneManager()->drawAll();
    m_device->getVideoDriver()->endScene();

    return m_frame;
}
