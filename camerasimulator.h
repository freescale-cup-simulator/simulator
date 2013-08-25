#ifndef CAMERASIMULATOR_H
#define CAMERASIMULATOR_H

#include <QApplication>
#include <QTimerEvent>
#include <QObject>
#include <QSize>
#include <QStringList>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QImage>
#include <QString>
#include <QDebug>
#include <track_model.h>
#include <irrlicht/irrlicht.h>
#include <math.h>

using namespace irr;
using namespace track_library;

//SCHURAKIN - Fantastic Moments

class CameraSimulator : public QObject
{
    Q_OBJECT
public:
    explicit CameraSimulator(const TrackModel &track,QSize deviceSize=QSize(128,128),QObject *parent = 0);
    const QList<scene::IAnimatedMeshSceneNode *> * tiles3d();
    core::vector3df getTargetPosition();
    ~CameraSimulator();
    
signals:

    void cameraResponse(const QByteArray & frame);
    void repaint(IrrlichtDevice * device);
    void unloaded();
public slots:
    void init();
    void onUnload();
    void onSimulatorResponse(const QMap<QString, QVariant> params);

private slots:
    void autoUpdateIrrlicht(IrrlichtDevice * device);

private:

    void timerEvent(QTimerEvent * event);

    int m_timerid;
    QSize m_size;
    bool m_capture;
    TrackModel m_track;
    QByteArray m_frame;
    QStringList m_supported_params;

    float m_radius;
    float m_position[6];

    IrrlichtDevice * m_device;

    scene::ISceneManager * m_manager;
    video::IVideoDriver * m_driver;
    scene::ICameraSceneNode * m_camera;
    scene::IBillboardSceneNode * m_target;
    video::ITexture * m_texture;
    QList<scene::IAnimatedMeshSceneNode *> m_tiles3d;
};

#endif // CAMERASIMULATOR_H
