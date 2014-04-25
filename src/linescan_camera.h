#ifndef LINESCAN_CAMERA_H
#define LINESCAN_CAMERA_H

#include <QByteArray>
#include <QImage>
#include <QRgb>
#include <QMutex>
#include <QMutexLocker>

#include <OgreCamera.h>
#include <OgreHardwarePixelBuffer.h>

#include <camera.h>
#include <global_renderer.h>
#include <ogre_engine.h>
#include <common.h>

class GlobalRenderer;

class LineScanCamera : public Camera, public RenderingObjectsUser
{
    Q_OBJECT
public:
    explicit LineScanCamera(Ogre::Camera * camera, QObject *parent = nullptr);
    ~LineScanCamera();
    void process(DataSet & data);

    inline void setRenderingObjects(RenderingObjects *ro)
    {
        RenderingObjectsUser::setRenderingObjects(ro);
        connect(ro->window, &QQuickWindow::beforeRendering, this,
                &LineScanCamera::update, Qt::DirectConnection);
    }

private slots:
    void update();
private:
    void setup();

    Ogre::TexturePtr m_texture;
    quint8 m_current_frame[CAMERA_FRAME_LEN];
    QMutex m_frame_locker;
};

#endif // LINESCAN_CAMERA_H
