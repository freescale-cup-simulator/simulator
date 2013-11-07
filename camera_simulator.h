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

#define BREAK_ON_STEP 2

class CameraSimulator : public QObject
{
    Q_OBJECT
public:
    explicit CameraSimulator(Ogre::Camera *camera, SharedImage *buffer, QObject *parent = 0);
    ~CameraSimulator();

    void process(DataSet & data);

private:

    void init();

    Ogre::Camera * m_camera;
    SharedImage * m_buffer;
    quint64 m_step;
    Ogre::Quaternion m_initial_orientation;
};

#endif // CAMERASIMULATOR_H
