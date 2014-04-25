#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <OgreQuaternion.h>
#include <OgreVector3.h>

constexpr int CAMERA_FRAME_LEN = 128;
constexpr int DATASET_VERSION = 1;

typedef struct
{
    quint8 camera_pixels[CAMERA_FRAME_LEN];

    struct
    {
        Ogre::Vector3 position;
        Ogre::Quaternion rotation;
    } camera;

    float current_wheel_angle;
    float desired_wheel_angle;
    float wheel_power_r;
    float wheel_power_l;
    float physics_timestep;
    float control_interval;
    qint32 line_position;
} DataSet;

Q_DECLARE_METATYPE(DataSet)

#endif // COMMON_H
