#include <simulation_runner.h>

Q_GLOBAL_STATIC(ResumeWaitCondition,staticResumeWaitCondition)

ResumeWaitCondition *ResumeWaitCondition::instance()
{
    return staticResumeWaitCondition;
}

bool ResumeWaitCondition::wait()
{
    return m_condition.wait(&m_mutex);
}

void ResumeWaitCondition::wake()
{
    m_condition.wakeAll();
}

SimulationRunner::SimulationRunner(QObject *parent)
    : QObject(parent)
    , m_state(SimulationRunner::Stopped)
    , m_control_interval(0.01)
    , m_physics_timestep(0.001)
    , m_car(0)
{
    setAutoDelete(false);
}

bool SimulationRunner::loadAlgorithmFile(const QUrl &file)
{
    if (m_state==SimulationRunner::Started||m_state==SimulationRunner::Paused)
        return false;
    if (!m_control_algorithm.isNull())
        m_control_algorithm.clear();
    auto ca = new ControlAlgorithm (this);
    if (!ca->loadFile(file.toLocalFile()))
    {
        delete ca;
        return false;
    }

    m_logger.setFileName(ca->getId() + "_"
                         + QDateTime::currentDateTime().toString("hhmmssddMMyyyy")
                         + ".dat");

    m_control_algorithm = QSharedPointer<ControlAlgorithm>(ca);
    return true;
}

/*TrackModel *SimulationRunner::loadTrack(const QString &track_path)
{
    if (m_state==SimulationRunner::Started||m_state==SimulationRunner::Paused)
    {
        qWarning("Will not change track while running");
        return nullptr;
    }

    m_track_model.clear();
    bool s = track_library::io::populateTrackFromFile(m_track_model,
                                                      track_path.toStdString());
    if (!s)
    {
        qWarning("Failed to populate track from %s, aborting",
                 track_path.toLocal8Bit().data());
        return nullptr;
    }
    return &m_track_model;
}*/

void SimulationRunner::run()
{
    if (!m_control_algorithm)
    {
        qWarning("Control algorithm not loaded, will not run");
        return;
    }
    Q_ASSERT(m_renderer);
    Q_ASSERT(m_car);

    m_state = SimulationRunner::Started;
    emit simulationStateChanged();

    auto cs = new CameraSimulator(m_renderer);
    auto ps = new PhysicsSimulation();
    auto vm = new VehicleModel;

    m_camera_simulator = QSharedPointer<CameraSimulator>(cs);
    m_physics_simulation = QSharedPointer<PhysicsSimulation>(ps);
    m_vehicle_model = QSharedPointer<VehicleModel>(vm);

    DataSet dataset;
    float physics_runtime;

    dataset.current_wheel_angle = 0;
    dataset.wheel_power_l = dataset.wheel_power_r = 0;
    dataset.physics_timestep = m_physics_timestep;
    dataset.control_interval = m_control_interval;
    dataset.line_position=64;

    Q_ASSERT(m_logger.beginWrite());

    while (m_state != SimulationRunner::Stopped)
    {
        m_logger<<dataset;

        if (m_state==SimulationRunner::Paused)
            ResumeWaitCondition::instance()->wait();

        physics_runtime = 0;
        while (physics_runtime < m_control_interval)
        {
            m_vehicle_model->process(dataset);
            m_physics_simulation->process(dataset);
            physics_runtime += m_physics_timestep;
        }

        m_camera_simulator->process(dataset);
        //m_car->process(dataset);
        m_control_algorithm->process(dataset);
    }

    m_logger.endWrite();

    m_state = SimulationState::Stopped;
    emit simulationStateChanged();

    qDebug("Simulation done!");
    m_camera_simulator.clear();
    m_vehicle_model.clear();
    m_physics_simulation.clear();
}

void SimulationRunner::pause()
{
    m_state = SimulationRunner::Paused;
    emit simulationStateChanged();
}

void SimulationRunner::resume()
{
    m_state = SimulationRunner::Started;
    emit simulationStateChanged();
    ResumeWaitCondition::instance()->wake();
}

void SimulationRunner::stop()
{
    m_state = SimulationRunner::Stopped;
    emit simulationStateChanged();
    ResumeWaitCondition::instance()->wake();
}
