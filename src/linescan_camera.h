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

/*!
 * \brief Simulates a line scan camera
 *
 * This class places an independent Ogre camera where the car's line scan camera
 * should be and provides a 128x1 frame to be passed to the control algorithm
 */
class LineScanCamera : public Camera, public RenderingObjectsUser
{
    Q_OBJECT
public:
    explicit LineScanCamera(Ogre::Camera * camera, QObject *parent = nullptr);
    ~LineScanCamera();
    void process(QPair<Ogre::Vector3, Ogre::Quaternion> state, quint8 line[]);

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
