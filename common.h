#ifndef COMMON_H
#define COMMON_H

#include <QMap>
#include <QString>
#include <QVariant>
#include <QtDebug>

//namespace parameters {

enum ParameterType {
    CameraPositionX,
    CameraPositionY,
    CameraPositionZ,
    CameraRotationX,
    CameraRotationY,
    CameraRotationZ,
    MovementAngle,
    WheelSpeedR,
    WheelSpeedL
};

//}

typedef QMap<ParameterType, QVariant> DataSet;

Q_DECLARE_METATYPE(DataSet)

#endif // COMMON_H
