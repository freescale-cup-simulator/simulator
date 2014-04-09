#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

#include <QSharedPointer>
#include <QRunnable>
#include <QThread>
#include <QWaitCondition>
#include <QDateTime>


#include <global_renderer.h>
#include <control_algorithm.h>
#include <physics_simulation.h>
#include <camera_simulator.h>
#include <vehicle_model.h>
#include <car3d.h>
#include <logger.h>
#include <track_io.h>
#include <config.h>

class GlobalRenderer;
class CameraSimulator;
class Car3D;

class ResumeWaitCondition : public QObject
{
    Q_OBJECT
public:
    ResumeWaitCondition(){}
    static ResumeWaitCondition * instance();
    bool wait();
    void wake();
private:
    QMutex m_mutex;
    QWaitCondition m_condition;
};

class SimulationRunner : public QObject, public QRunnable
{
    Q_OBJECT
    Q_PROPERTY(float controlInterval READ controlInterval
               WRITE setControlInterval
               NOTIFY controlIntervalChanged)
    Q_PROPERTY(float physicsTimeStep READ physicsTimeStep
               WRITE setPhysicsTimeStep
               NOTIFY physicsTimeStepChanged)
    Q_PROPERTY(SimulationState simulationState READ getState NOTIFY simulationStateChanged)
    Q_ENUMS(SimulationState)
public:
    enum SimulationState {Stopped, Paused, Started};
    explicit SimulationRunner(QObject * parent = nullptr);

    track_library::TrackModel * loadTrack(const QString & track_path);

    Q_INVOKABLE bool loadAlgorithmFile(const QUrl & file);

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

    inline void setCar(Car3D * car) { m_car = car; }
    inline void setRenderer(GlobalRenderer * renderer) { m_renderer = renderer; }

    inline SimulationState getState() { return m_state; }

public slots:
    void run();
    void pause();
    void resume();
    void stop();
signals:
    void controlIntervalChanged();
    void physicsTimeStepChanged();
    void simulationStateChanged();
private:
    SimulationState m_state;

    float m_control_interval;
    float m_physics_timestep;

    track_library::TrackModel m_track_model;
    QSharedPointer<PhysicsSimulation> m_physics_simulation;
    QSharedPointer<CameraSimulator> m_camera_simulator;
    QSharedPointer<ControlAlgorithm> m_control_algorithm;
    QSharedPointer<VehicleModel> m_vehicle_model;
    GlobalRenderer * m_renderer;
    Logger m_logger;
    Car3D * m_car;
};

#endif // SIMULATION_RUNNER_H
