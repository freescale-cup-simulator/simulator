#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <OgreQuaternion.h>
#include <OgreVector3.h>

constexpr int CAMERA_FRAME_LEN = 128;
constexpr float CONTROL_INTERVAL = 0.001;
constexpr float PHYSICS_TIMESTEP = 0.00005;

namespace DataSetValues
{

enum
{
    CURRENT_WHEEL_ANGLE = 0,
    DESIRED_WHEEL_ANGLE,
    WHEEL_POWER_R,
    WHEEL_POWER_L,
    LINE_POSITION,
    TIMESTAMP,

    VALUE_MAX
};

}

typedef float DataSet[DataSetValues::VALUE_MAX];

#endif // COMMON_H
