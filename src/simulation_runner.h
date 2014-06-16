#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

#include <QSharedPointer>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QWaitCondition>
#include <QDateTime>
#include <QPair>

#include <global_renderer.h>
#include <control_algorithm.h>
#include <physics_simulation.h>
#include <linescan_camera.h>
#include <vehicle.h>
#include <logger.h>
#include <track_io.h>
#include <config.h>

class GlobalRenderer;
class LineScanCamera;

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


/*!
 * \brief Manages the simulation process
 *
 * The simulation has two main parameters: controlInterval and PhysicsTimeStep.
 * physicsTimeStep is time, in seconds, by which PhysicsSimulation advances the
 * state of the physical world; if an object is moving at 1 m/s and
 * physicsTimeStep = 1 then the object will move by 1 meter per simulation step.
 * Lower physicsTimeStep allows for more precision but results in slower simulation.
 *
 * controlInterval is the amount of time between calls to the control algorithm.
 */
class SimulationRunner : public QObject, public QRunnable
{
    Q_OBJECT
    Q_PROPERTY(SimulationState simulationState READ getState
               NOTIFY simulationStateChanged)
    Q_ENUMS(SimulationState)
public:
    enum SimulationState {Stopped, Paused, Started};
    explicit SimulationRunner(QObject * parent = nullptr);

    /*!
     * \brief Loads a control algorithm file, see ControlAlgorithm::loadFile()
     */
    Q_INVOKABLE bool loadAlgorithmFile(const QUrl & file);

    inline void setVehicle(Vehicle * v)
    {
        m_vehicle = v;
    }

    inline void setRenderer(GlobalRenderer * renderer) { m_renderer = renderer; }

    inline SimulationState getState() { return m_state; }

    inline void setPhysicsSimulation(PhysicsSimulation *ps)
    {
        m_physics_simulation = ps;
    }

    inline void setCameraSimulator(LineScanCamera * cm) { m_linescan_camera = cm; }

    /*!
     * \brief starts the simulation thread
     */
    Q_INVOKABLE void startThread()
    {
        /*
         * we can only create the vehicle here (and not in run()) because all assets
         * are created on the main (GUI) thread, and run() is executed in a different
         * thread; startThread(), however, is called from GUI
         */
        m_vehicle->create();
        QThreadPool::globalInstance()->start(this);
    }

    /*!
     * \brief Overloaded from QRunnable; this method is executed in the
     *        simulation thread
     */
    void run();

public slots:
    void setState(SimulationState state);
signals:
    void simulationStateChanged();
    void simulationStopped();
private:
    SimulationState m_state;

    LineScanCamera * m_linescan_camera;
    QSharedPointer<ControlAlgorithm> m_control_algorithm;

    Vehicle * m_vehicle;
    PhysicsSimulation * m_physics_simulation;
    GlobalRenderer * m_renderer;
    Logger m_logger;
};

#endif // SIMULATION_RUNNER_H
