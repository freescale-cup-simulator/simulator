#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

#include <QSharedPointer>
#include <QRunnable>
#include <QThread>

#include <control_algorithm.h>
#include <physics_simulation.h>
#include <camera_simulator.h>
#include <track_io.h>
#include <config.h>

class SimulationRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit SimulationRunner(QObject * parent = nullptr);
    bool loadAlgorithmFile(const QString & file);
    bool loadTrack(const QString & track_path);
    void run();
    void stop();
private:
    bool m_running;

    track_library::TrackModel m_track_model;
    QSharedPointer<PhysicsSimulation> m_physics_simulation;
    QSharedPointer<CameraSimulator> m_camera_simulator;
    QSharedPointer<ControlAlgorithm> m_control_algorithm;
};

#endif // SIMULATION_RUNNER_H
