#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

#include <QSharedPointer>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QWaitCondition>
#include <QDateTime>

#include <global_renderer.h>
#include <control_algorithm.h>
#include <physics_simulation.h>
#include <linescan_camera.h>
#include <vehicle_model.h>
#include <vehicle.h>
#include <logger.h>
#include <track_io.h>
#include <config.h>

class GlobalRenderer;
class LineScanCamera;
class Vehicle;

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
    Q_PROPERTY(SimulationState simulationState READ getState
               NOTIFY simulationStateChanged)
    Q_ENUMS(SimulationState)
public:
    enum SimulationState {Stopped, Paused, Started};
    explicit SimulationRunner(QObject * parent = nullptr);

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

    inline void setVehicle(Vehicle * v) { m_vehicle = v; }
    inline void setRenderer(GlobalRenderer * renderer) { m_renderer = renderer; }

    inline SimulationState getState() { return m_state; }

    inline void setPhysicsSimulation(PhysicsSimulation *ps)
    {
        m_physics_simulation = ps;
    }

    inline void setCameraSimulator(LineScanCamera * cm) { m_linescan_camera = cm; }

    Q_INVOKABLE void startThread()
    {
        QThreadPool::globalInstance()->start(this);
    }

public slots:
    void setState(SimulationState state);
    void run();
signals:
    void controlIntervalChanged();
    void physicsTimeStepChanged();
    void simulationStateChanged();
    void simulationStopped();
private:
    SimulationState m_state;

    float m_control_interval;
    float m_physics_timestep;

    track_library::TrackModel m_track_model;
    LineScanCamera * m_linescan_camera;
    QSharedPointer<ControlAlgorithm> m_control_algorithm;

    Vehicle * m_vehicle;
    PhysicsSimulation * m_physics_simulation;
    GlobalRenderer * m_renderer;
    Logger m_logger;
};

#endif // SIMULATION_RUNNER_H
