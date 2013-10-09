#include <simulation_runner.h>

SimulationRunner::SimulationRunner(QObject *parent)
    : QObject(parent)
    , m_running(false)
    , m_control_interval(0.1)
    , m_physics_timestep(8e-3)
{
    setAutoDelete(false);
}

bool SimulationRunner::loadAlgorithmFile(const QString &file)
{
    if (m_running)
        return false;
    auto ca = new ControlAlgorithm (this);
    if (!ca->loadFile(file))
    {
        delete ca;
        return false;
    }
    m_control_algorithm = QSharedPointer<ControlAlgorithm>(ca);
    return true;
}

bool SimulationRunner::loadTrack(const QString &track_path)
{
    if (m_running)
    {
        qWarning("Will not change track while running");
        return false;
    }

    m_track_model.clear();
    bool s = track_library::io::populateTrackFromFile(m_track_model,
                                                      track_path.toStdString());
    if (!s)
    {
        qWarning("Failed to populate track from %s, aborting",
                 track_path.toLocal8Bit().data());
        return false;
    }
    return true;
}

void SimulationRunner::run()
{
    if (!m_control_algorithm)
    {
        qWarning("Control algorithm not loaded, will not run");
        return;
    }

    auto cs = new CameraSimulator(m_track_model, QSize(128, 128));
    auto ps = new PhysicsSimulation(m_track_model);
    m_camera_simulator = QSharedPointer<CameraSimulator>(cs);
    m_physics_simulation = QSharedPointer<PhysicsSimulation>(ps);

    m_running = true;
    DataSet dataset;
    float physics_runtime;

    dataset.physics_timestep = m_physics_timestep;
    dataset.control_interval = m_control_interval;
    m_control_algorithm->process(dataset);

    while (m_running)
    {
        physics_runtime = 0;
        while (physics_runtime < m_control_interval)
        {
            m_physics_simulation->process(dataset);
            // run vehicle model code here

            physics_runtime += m_physics_timestep;
        }
        m_camera_simulator->process(dataset);
        m_control_algorithm->process(dataset);
    }
    qDebug("Simulation done!");
}

void SimulationRunner::stop()
{
    m_running = false;
}
