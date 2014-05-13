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
    , m_control_interval(0.001)
    , m_physics_timestep(0.00005)
{
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

void SimulationRunner::setState(SimulationRunner::SimulationState state)
{
    m_state = state;
    if (state != SimulationState::Paused)
        ResumeWaitCondition::instance()->wake();
    if (state == SimulationState::Stopped)
        emit simulationStopped();
    emit simulationStateChanged();
}

void SimulationRunner::run()
{
    if (!m_control_algorithm)
    {
        qWarning("Control algorithm not loaded, will not run");
        return;
    }
    Q_ASSERT(m_renderer);
    Q_ASSERT(m_vehicle);
    Q_ASSERT(m_linescan_camera);

    m_state = SimulationRunner::Started;
    emit simulationStateChanged();

    DataSet dataset;
    float physics_runtime;

    dataset.current_wheel_angle = 0;
    dataset.wheel_power_l = dataset.wheel_power_r = 0;
    dataset.physics_timestep = m_physics_timestep;
    dataset.control_interval = m_control_interval;
    dataset.line_position=64;

    Q_ASSERT(m_logger.beginWrite());

    m_vehicle->process(dataset);

    while (m_state != SimulationRunner::Stopped)
    {
        m_logger << dataset;

        if (m_state==SimulationRunner::Paused)
            ResumeWaitCondition::instance()->wait();

        physics_runtime = 0;
        while (physics_runtime < m_control_interval)
        {
            m_physics_simulation->process(dataset);
            m_vehicle->process(dataset);
            physics_runtime += m_physics_timestep;
        }
        m_linescan_camera->process(dataset);
        m_control_algorithm->process(dataset);

    }

    m_logger.endWrite();

    m_state = SimulationState::Stopped;
    emit simulationStateChanged();

    qDebug("Simulation done!");
}
