#ifndef CAMERASIMULATOR_H
#define CAMERASIMULATOR_H

#include <QSize>
#include <QByteArray>
#include <QList>
#include <QImage>
#include <QString>
#include <QDebug>
#include <QRgb>

#include <OgreCamera.h>

#include <shared_image.h>
#include <cmath>
#include <common.h>

class CameraSimulator : public QObject
{
    Q_OBJECT
public:
    explicit CameraSimulator(Ogre::Camera *camera, SharedImage *buffer, QObject *parent = 0);
    ~CameraSimulator();

    void process(DataSet & data);

private:

    Ogre::Camera * m_camera;
    SharedImage * m_buffer;
};

#endif // CAMERASIMULATOR_H
