#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

#include <QSharedPointer>
#include <QRunnable>
#include <QThread>
#include <QSemaphore>
#include <QFileInfo>

#include <global_renderer.h>
#include <control_algorithm.h>
#include <physics_simulation.h>
#include <camera_simulator.h>
#include <vehicle_model.h>
#include <logger.h>
#include <track_io.h>
#include <config.h>


#define VIRTUAL_CAMERAS_COUNT 1

class SimulationRunner : public QObject, public QRunnable
{
    Q_OBJECT
    Q_PROPERTY(float controlInterval READ controlInterval
               WRITE setControlInterval
               NOTIFY controlIntervalChanged)
    Q_PROPERTY(float physicsTimeStep READ physicsTimeStep
               WRITE setPhysicsTimeStep
               NOTIFY physicsTimeStepChanged)
public:
    explicit SimulationRunner(GlobalRenderer * renderer,QObject * parent = nullptr);
    bool loadAlgorithmFile(const QString & file);
    track_library::TrackModel * loadTrack(const QString & track_path);
    void run();

    inline float controlInterval() { return m_control_interval; }
    inline float physicsTimeStep() { return m_physics_timestep; }

    inline void setControlInterval(float v)
    {
        m_control_interval = v;
        controlIntervalChanged();
    }

    inline void setPhysicsTimeStep(float v)
    {
        m_physics_timestep = v;
        physicsTimeStepChanged();
    }
public slots:
    void stop();

signals:

    void controlIntervalChanged();
    void physicsTimeStepChanged();
    void simulationStopped();

private:
    bool m_running;
    float m_control_interval;
    float m_physics_timestep;

    track_library::TrackModel m_track_model;
    QSharedPointer<PhysicsSimulation> m_physics_simulation;
    QSharedPointer<CameraSimulator> m_camera_simulator;
    QSharedPointer<ControlAlgorithm> m_control_algorithm;
    QSharedPointer<VehicleModel> m_vehicle_model;
    GlobalRenderer * m_renderer;
    Logger m_logger;
    QSemaphore m_cameras_syncronizator;
};

#endif // SIMULATION_RUNNER_H
