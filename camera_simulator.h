#ifndef CAMERASIMULATOR_H
#define CAMERASIMULATOR_H

#include <QSize>
#include <QByteArray>
#include <QList>
#include <QImage>
#include <QString>
#include <QDebug>

#include <irrlicht/irrlicht.h>
#include <cmath>

#include <config.h>
#include <common.h>
#include <track_model.h>


using namespace irr;
using namespace track_library;

//SCHURAKIN - Fantastic Moments

class CameraSimulator : public QObject
{
    Q_OBJECT
public:
    explicit CameraSimulator(const TrackModel &track,
                             QSize deviceSize = QSize(128, 128),
                             QObject *parent = 0);
    ~CameraSimulator();
    core::vector3df getTargetPosition();
    QByteArray onSimulatorResponse(const DataSet & params);
private:
    void init();

    core::dimension2du m_device_size;
    const TrackModel & m_track_model;

    IrrlichtDevice * m_device;
    scene::ISceneManager * m_scene_manager;
    video::IVideoDriver * m_video_driver;
    scene::ICameraSceneNode * m_camera;

    scene::IBillboardSceneNode * m_target;
    video::ITexture * m_frame_texture;
};

#endif // CAMERASIMULATOR_H
