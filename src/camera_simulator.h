#ifndef CAMERASIMULATOR_H
#define CAMERASIMULATOR_H

#include <QSize>
#include <QByteArray>
#include <QList>
#include <QImage>
#include <QString>
#include <QDebug>
#include <QRgb>
#include <QMutex>
#include <QMutexLocker>

#include <OgreCamera.h>
#include <OgreHardwarePixelBuffer.h>
#include <global_renderer.h>
#include <ogre_engine.h>
#include <cmath>
#include <common.h>

class GlobalRenderer;

class CameraSimulator : public QObject
{
    Q_OBJECT
public:

    explicit CameraSimulator(GlobalRenderer * renderer, QObject *parent = 0);
    ~CameraSimulator();
    void process(DataSet & data);

private slots:
    void onUpdate();
private:
    OgreEngine * m_engine;
    Ogre::RenderTexture * m_renderTarget;
    Ogre::Camera * m_camera;
    quint8 * m_frame;
    QMutex m_frame_locker,m_safe_destruct;
    Ogre::SceneManager * m_manager;
    GlobalRenderer * m_renderer;
};

#endif // CAMERASIMULATOR_H
