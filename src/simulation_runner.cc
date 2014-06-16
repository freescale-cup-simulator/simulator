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

    m_control_algorithm = QSharedPointer<ControlAlgorithm>(ca);
    return true;
}

void SimulationRunner::setState(SimulationRunner::SimulationState state)
{
    m_state = state;
    if (state == Started || state == Stopped)
        ResumeWaitCondition::instance()->wake();

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

    setState(Started);

    DataSet dataset;
    float physics_runtime;
    QPair<Ogre::Vector3, Ogre::Quaternion> camera_state;
    quint8 scanned_line[CAMERA_FRAME_LEN];

    std::memset(&dataset, 0, sizeof(DataSet));
    dataset[DataSetValues::LINE_POSITION] = 64;

    m_vehicle->process(dataset, camera_state);

    while (m_state != SimulationRunner::Stopped)
    {
        if (m_state==SimulationRunner::Paused)
        {
            ResumeWaitCondition::instance()->wait();
            continue;
        }

        physics_runtime = 0;
        while (physics_runtime < CONTROL_INTERVAL)
        {
            m_physics_simulation->process();
            m_vehicle->process(dataset, camera_state);
            physics_runtime += PHYSICS_TIMESTEP;
        }

        m_linescan_camera->process(camera_state, scanned_line);
        m_control_algorithm->process(dataset, scanned_line);

        dataset[DataSetValues::TIMESTAMP] += CONTROL_INTERVAL;
        m_logger.appendDataset(dataset);
    }

    QString fn = m_control_algorithm->getId() + "_"
            + QDateTime::currentDateTime().toString("hhmmssddMMyyyy")
            + ".hdf";

    if (!m_logger.saveToFile(fn))
    {
        qWarning("SimulationRunner: failed to save log file");
    }

    emit simulationStopped();
    m_control_algorithm.clear();

    qDebug("Simulation done!");
}
