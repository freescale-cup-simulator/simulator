#ifndef VEHICLE_MODEL_H
#define VEHICLE_MODEL_H

#include <QObject>
#include <QDebug>
#include <common.h>

class VehicleModel : public QObject
{
    Q_OBJECT
public:
    VehicleModel(QObject * parent = nullptr);
    ~VehicleModel();
    void process(DataSet & data);
private:
    // for Futaba FUTM0032 servo
    static constexpr float DEGREES_PER_SECOND = 60.0 / 0.2;
};

#endif // VEHICLE_MODEL_H
