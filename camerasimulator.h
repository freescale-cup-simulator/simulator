#ifndef CAMERASIMULATOR_H
#define CAMERASIMULATOR_H

#include <QApplication>
#include <QTimerEvent>
#include <QObject>
#include <QSize>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QImage>
#include <QString>

#include <track_io.h>
#include <track_model.h>
#include <irrlicht/irrlicht.h>

using namespace irr;
using namespace track_library;

//SCHURAKIN - Fantastic Moments

class CameraSimulator : public QObject
{
    Q_OBJECT
public:
    explicit CameraSimulator(QString trackFile,QSize deviceSize=QSize(128,128),QObject *parent = 0);
    ~CameraSimulator();
    
signals:

    void cameraResponse(QMap<QString,QVariant> params);
    void repaint(IrrlichtDevice * device);
    void unloaded();
public slots:
    void init();
    void onUnload();
    void onSimulatorResponse(QMap<QString,QVariant> params);

private slots:
    void autoUpdateIrrlicht(IrrlichtDevice * device);

private:

    void timerEvent(QTimerEvent * event);

    int m_timerid;
    QSize m_size;
    bool m_capture;
    TrackModel m_track;
    QByteArray m_frame;

    IrrlichtDevice * m_device;

    scene::ISceneManager * m_manager;
    video::IVideoDriver * m_driver;
    scene::ICameraSceneNode * m_camera;
    scene::IBillboardSceneNode * m_target;
    video::ITexture * m_texture;

};

#endif // CAMERASIMULATOR_H
