#ifndef COMMON_H
#define COMMON_H

#include <QVector3D>
#include <QQuaternion>
#include <QDebug>

constexpr int CAMERA_FRAME_LEN = 128;
constexpr quint32 DATASET_VERSION=0x1;

typedef struct
{
    QVector3D camera_position;
    QVector3D camera_rotation;
    QQuaternion camera_rotation_quat;
    quint8 camera_pixels[CAMERA_FRAME_LEN];
    float current_wheel_angle;
    float desired_wheel_angle;
    float wheel_power_r;
    float wheel_power_l;
    float physics_timestep;
    float control_interval;
} DataSet;

Q_DECLARE_METATYPE(DataSet)

#endif // COMMON_H
