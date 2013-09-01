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
    const QList<scene::IAnimatedMeshSceneNode *> * tiles3d();
    core::vector3df getTargetPosition();
    ~CameraSimulator();
signals:
    void cameraResponse(const QByteArray & frame);
    void unloaded();
public slots:
    void onUnload();
    void onSimulatorResponse(const DataSet & params);

private:
    void init();

    QSize m_size;
    bool m_capture;
    const TrackModel & m_track;
    QByteArray m_frame;

    float m_radius;

    IrrlichtDevice * m_device;

    scene::ISceneManager * m_manager;
    video::IVideoDriver * m_driver;
    scene::ICameraSceneNode * m_camera;
    scene::IBillboardSceneNode * m_target;
    video::ITexture * m_texture;
    QList<scene::IAnimatedMeshSceneNode *> m_tiles3d;
};

#endif // CAMERASIMULATOR_H
