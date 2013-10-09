#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QString>
#include <QVector3D>
#include <QtDebug>

typedef struct
{
    QVector3D camera_position;
    QVector3D camera_rotation;
    QByteArray camera_pixels;
    float wheel_angle;
    float wheel_power_r;
    float wheel_power_l;
    float physics_timestep;
    float control_interval;
} DataSet;

Q_DECLARE_METATYPE(DataSet)

#endif // COMMON_H
