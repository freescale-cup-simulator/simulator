#include "camerasimulator.h"
#define PI 3.14159265
CameraSimulator::CameraSimulator(const TrackModel &track, QSize deviceSize, QObject *parent) :
    QObject(parent)
{
    m_size=deviceSize;
    m_track=track;
    m_radius=1;
    m_supported_params<<"camX"<<"camY"<<"camZ"<<"rotateX"<<"rotateY"<<"rotateZ";
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
    video::E_DRIVER_TYPE driverType;
    driverType = video::EDT_OPENGL;

    Q_ASSERT(m_device=createDevice(driverType, core::dimension2d<u32>(m_size.width(), m_size.height())));

    connect( this, SIGNAL(repaint(IrrlichtDevice*)),
             this, SLOT(autoUpdateIrrlicht(IrrlichtDevice*)));

    m_manager=m_device->getSceneManager();
    m_driver=m_device->getVideoDriver();
    m_camera=m_manager->addCameraSceneNode();

    m_camera->setPosition(core::vector3df(0, 4, 0));
    m_camera->setRotation(core::vector3df(0,0,0));
    //m_camera->setTarget(core::vector3df(0,0,-2));
    m_camera->setUpVector(core::vector3df(0,1,0));
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
                                        core::dimension2d<f32>(m_track.width()/2, m_track.height()/2));
    auto plane = m_manager->addAnimatedMeshSceneNode(planeMesh);
    plane->setMaterialTexture(0, m_driver->getTexture("images/texture.jpg"));
    plane->setMaterialFlag(video::EMF_LIGHTING, false);

    plane->setPosition(core::vector3df(m_track.width()/2, 0, m_track.height()/2));
    //float offsetX,offsetZ;
    //offsetX=0.5;
    //offsetZ=m_track.height()-0.5;
    plane->setRotation(core::vector3df(0, 0, 0));
    QList<Tile> tiles=QList<Tile>::fromStdList(m_track.tiles());
    //int i=0;
    foreach (Tile tile, tiles) {
        QString modelName;
        QString textureName;
        switch(tile.type())
        {
        case Tile::Line:
            modelName="models/line.dae";
            textureName="images/line.jpg";
            break;
        case Tile::Crossing:
            modelName="models/crossing.dae";
            textureName="images/crossing.png";
            break;
        case Tile::Start:
            modelName="models/start.dae";
            textureName="images/start.png";
            break;
        case Tile::Saw:
            modelName="models/saw.dae";
            textureName="images/saw.png";
            break;
        case Tile::Turn:
            modelName="models/turn.dae";
            textureName="images/turn.png";
            break;
        default:
            modelName="models/empty.dae";
            textureName="images/empty.png";
        }
        auto tileMesh=m_manager->getMesh(modelName.toStdString().c_str());
        /*auto tileMesh=m_manager->addHillPlaneMesh("", core::dimension2d<f32>(1, 1),
                                                  core::dimension2d<u32>(1, 1),
                                                  0, 0, core::dimension2d<f32>(0, 0),core::dimension2d<f32>(1, 1));*/
        auto tilePlane=m_manager->addAnimatedMeshSceneNode(tileMesh);
        //tilePlane->setMaterialTexture(0,m_driver->getTexture(textureName.toStdString().c_str()));
        tilePlane->setScale(core::vector3df(0.5,0,0.5));
        tilePlane->setMaterialFlag(video::EMF_LIGHTING, false);
        tilePlane->setRotation(core::vector3df(0,tile.rotation()-90,0));
        tilePlane->setPosition(core::vector3df(tile.y()+0.5,0.01f,tile.x()+0.5));
        m_tiles3d<<tilePlane;
        //i++;
        //if(i==2)
        //    break;
    }
    m_target=m_manager->addBillboardSceneNode(0,core::dimension2df(0.1,0.1),core::vector3df(0,0,0));
    m_target->setMaterialFlag(video::EMF_LIGHTING, false);
    m_target->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
    m_target->setMaterialTexture(0,m_driver->getTexture("images/target.png"));
    m_texture=m_driver->addRenderTargetTexture(core::dimension2d<u32>(m_size.width(),m_size.height()), "frame",video::ECF_R8G8B8);
    //m_capture=true;
    m_camera->bindTargetAndRotation(true);
    m_timerid=startTimer(0);
}

void CameraSimulator::onUnload()
{
    m_device->drop();
    killTimer(m_timerid);
    emit unloaded();
}

void CameraSimulator::onSimulatorResponse(const QMap<QString, QVariant> params)
{
    int i=0;
    foreach (QString key, m_supported_params) {
        QVariant param=params[key];
        if(param.canConvert(QMetaType::Float))
            m_position[i]=param.toFloat();
        else
        {
            m_position[i]=0;
            qDebug()<<"CameraSimulator: Can't convert "<<key<<" to Float";
        }
        //qDebug()<<params[key];
        i++;
    }
    m_camera->setPosition(core::vector3df(m_position[0],m_position[1],m_position[2]));
    m_camera->updateAbsolutePosition();
    m_camera->setRotation(core::vector3df(m_position[3],m_position[4],m_position[5]));
    m_camera->updateAbsolutePosition();
    m_target->setPosition(m_camera->getTarget());
    m_target->updateAbsolutePosition();

    //qDebug()<<m_camera->getUpVector().X<<m_camera->getUpVector().Y<<m_camera->getUpVector().Z;
    m_capture=true;
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
            m_frame.append(qRed(line[i]));
        m_texture->unlock();
        m_capture=false;
        //QMap<QString,QVariant> params;
        //params["frame"]=QVariant(m_frame);
        emit cameraResponse(m_frame);
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
