#include "camerasimulator.h"

CameraSimulator::CameraSimulator(QString trackFile,QSize deviceSize,QObject *parent) :
    QObject(parent)
{
    m_size=deviceSize;
    Q_ASSERT(track_library::io::populateTrackFromFile(m_track,trackFile.toStdString()));
}

CameraSimulator::~CameraSimulator()
{

}
core::vector3df Rotation;
// this one will rotate node around absolute X and Y axis
// if you try to rotate around Z (Q,W keys) strange things will happen
void rotateAround1B(scene::ISceneNode *node, const core::vector3df &origin,
   const core::vector3df &angle)
{
   // current relative position of node against origin
   core::vector3df pos = node->getPosition() - origin;

   // new rotation of node
   Rotation += angle;

   // position were node is when unrotated
   f32 distance = pos.getLength();
   core::vector3df newPos(0,0,distance); // this assumes Z axis points "forward"

   // now rotate position vector with new rotation
   // to find out new position of node
   core::matrix4 m;
   m.setRotationDegrees(Rotation);
   m.rotateVect(newPos);
   // get it absolute
   newPos += origin;

   // position node
   node->setPosition(newPos);

   //cout << "X: " << Rotation.X << ", Y: " << Rotation.Y << ", Z: " << Rotation.Z
   //   <<endl;
}

void CameraSimulator::init()
{
    video::E_DRIVER_TYPE driverType;
    driverType = video::EDT_OPENGL;

    Q_ASSERT(m_device=createDevice(driverType, core::dimension2d<u32>(m_size.width(), m_size.height())));

    connect( this, SIGNAL(repaint(IrrlichtDevice*)),
             this, SLOT(autoUpdateIrrlicht(IrrlichtDevice*)));

    m_manager=m_device->getSceneManager();
    m_driver=m_device->getVideoDriver();
    m_camera=m_manager->addCameraSceneNode();

    m_camera->setPosition(core::vector3df(0, 2, -2));
    m_camera->setRotation(core::vector3df(0,0,0));
    m_camera->setTarget(core::vector3df(0,0,-2));
    m_camera->setUpVector(core::vector3df(0,0,1));
    m_camera->updateAbsolutePosition();

    video::ITexture *front=m_driver->getTexture("images/skyrender0001.bmp");
    video::ITexture *right=m_driver->getTexture("images/skyrender0002.bmp");
    video::ITexture *top=m_driver->getTexture("images/skyrender0003.bmp");
    video::ITexture *back=m_driver->getTexture("images/skyrender0004.bmp");
    video::ITexture *left=m_driver->getTexture("images/skyrender0005.bmp");

    m_manager->addSkyBoxSceneNode(top,nullptr,left,right,front,back);

    auto planeMesh = m_manager->addHillPlaneMesh("plane", core::dimension2d<f32>(1, 1),
                                        core::dimension2d<u32>(m_track.width(), m_track.height()),
                                        0, 0, core::dimension2d<f32>(0, 0),
                                        core::dimension2d<f32>(4, 4));
    auto plane = m_manager->addAnimatedMeshSceneNode(planeMesh);
    plane->setMaterialTexture(0, m_driver->getTexture("images/texture.jpg"));
    plane->setMaterialFlag(video::EMF_LIGHTING, false);

    plane->setPosition(core::vector3df(0, 0, 0));
    float offsetX,offsetZ;
    offsetX=-m_track.width()/2+0.5f;
    offsetZ=m_track.width()/2-0.5f;
    plane->setRotation(core::vector3df(0, 0, 0));
    QList<Tile> tiles=QList<Tile>::fromStdList(m_track.tiles());
    foreach (Tile tile, tiles) {
        QString modelName;
        switch(tile.type())
        {
        case Tile::Line:
            modelName="models/line.dae";
            break;
        case Tile::Crossing:
            modelName="models/crossing.dae";
            break;
        case Tile::Start:
            modelName="models/start.dae";
            break;
        case Tile::Saw:
            modelName="models/saw.dae";
            break;
        case Tile::Turn:
            modelName="models/turn.dae";
            break;
        default:
            modelName="models/empty.dae";
        }
        auto tileMesh=m_manager->getMesh(modelName.toStdString().c_str());/*m_manager->addHillPlaneMesh("", core::dimension2d<f32>(1, 1),
                                                  core::dimension2d<u32>(1, 1),
                                                  0, 0, core::dimension2d<f32>(0, 0),core::dimension2d<f32>(1, 1));*/
        auto tilePlane=m_manager->addAnimatedMeshSceneNode(tileMesh);
        tilePlane->setScale(core::vector3df(0.5,0.5,0.5));
        tilePlane->setMaterialFlag(video::EMF_LIGHTING, false);
        //rotateAround1B(tilePlane,core::vector3df(tile.x()+offsetX+0.5f,0.04f,tile.y()+offsetZ-0.5f),core::vector3df(0,-tile.rotation(),0));
        tilePlane->setRotation(core::vector3df(0,tile.rotation(),0));
        tilePlane->setPosition(core::vector3df(tile.x()+offsetX,0.04f,-tile.y()+offsetZ));
        //tilePlane->setMaterialTexture(0, m_driver->getTexture(textureName.toStdString().c_str()));
    }

    m_texture=m_driver->addRenderTargetTexture(core::dimension2d<u32>(m_size.width(),m_size.height()), "frame",video::ECF_R8G8B8);

    m_timerid=startTimer(0);
}

void CameraSimulator::onUnload()
{
    m_device->drop();
    killTimer(m_timerid);
    emit unloaded();
}

void CameraSimulator::onSimulatorResponse(QMap<QString, QVariant> params)
{

}


void CameraSimulator::autoUpdateIrrlicht(IrrlichtDevice *device)
{
    device->getTimer()->tick();
    irr::video::SColor color (0xC,0xC,0xC,0);
    device->getVideoDriver()->beginScene( true, true, color );
    if(m_capture)
    {
        m_driver->setRenderTarget(m_texture, true, true, video::SColor(0,0,0,255));
        m_target->setVisible(false);
        device->getSceneManager()->drawAll();
        m_driver->setRenderTarget(0, true, true, 0);
        m_target->setVisible(true);
        void* imageData = m_texture->lock();
        m_frame.clear();
        QImage qimg((uchar*)imageData, m_size.width(),m_size.height(),QImage::Format_RGB32);
        QRgb * line=(QRgb *)qimg.scanLine(m_size.height()/2);
        for(int i=0;i<m_size.width();i++)
            m_frame.append(qGray(line[i]));
        m_texture->unlock();
        m_capture=false;
        QMap<QString,QVariant> params;
        params["frame"]=QVariant(m_frame);
        emit cameraResponse(params);
    }
    device->getSceneManager()->drawAll();
    device->getVideoDriver()->endScene();
    QApplication::processEvents();
}

void CameraSimulator::timerEvent(QTimerEvent *event)
{
    if ( m_device != 0 )
    {
        emit repaint(m_device);
    }
    event->accept();
}
