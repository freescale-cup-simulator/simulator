#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

#include <common.h>
#include <camera_simulator.h>
#include <physics_simulation.h>

class SimulationRunner : public QObject
{
public:
    explicit SimulationRunner(QObject * parent = nullptr);
    void run();
};

#endif // SIMULATION_RUNNER_H
