#include <vehicle_model.h>

VehicleModel::VehicleModel(QObject * parent)
    : QObject(parent)
{
}

void VehicleModel::process(const DataSet & target, DataSet & output)
{
    output = target;
    if (target.wheel_angle < m_last_output.wheel_angle)
        m_last_output.wheel_angle -= DEGREES_PER_SECOND * target.physics_timestep;
    else if (target.wheel_angle > m_last_output.wheel_angle)
        m_last_output.wheel_angle += DEGREES_PER_SECOND * target.physics_timestep;
    output.wheel_angle = m_last_output.wheel_angle;
    qDebug() << target.wheel_angle << output.wheel_angle;
}
