#include <vehicle_model.h>

VehicleModel::VehicleModel(QObject * parent)
    : QObject(parent)
{
}

void VehicleModel::process(DataSet & d)
{
    const float dps = DEGREES_PER_SECOND * d.physics_timestep;

    // servo will reach desired angle during current step
    if (std::abs(d.desired_wheel_angle - d.current_wheel_angle) < dps)
    {
        d.current_wheel_angle = d.desired_wheel_angle;
    }
    else if (d.desired_wheel_angle < d.current_wheel_angle)
    {
        d.current_wheel_angle -= dps;
    }
    else
    {
        d.current_wheel_angle += dps;
    }
    qDebug() << d.current_wheel_angle << d.desired_wheel_angle;
}
