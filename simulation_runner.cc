#include <simulation_runner.h>

SimulationRunner::SimulationRunner(QObject *parent)
    : QObject(parent)
    , m_running(false)
{
    setAutoDelete(false);
}

bool SimulationRunner::loadAlgorithmFile(const QString &file)
{
    if (m_running)
        return false;
    auto ca = new ControlAlgorithm (this);
    m_control_algorithm = QSharedPointer<ControlAlgorithm>(ca);
    return ca->loadFile(file);
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

    auto cs = new CameraSimulator(m_track_model);
    auto ps = new PhysicsSimulation(m_track_model);
    m_camera_simulator = QSharedPointer<CameraSimulator>(cs);
    m_physics_simulation = QSharedPointer<PhysicsSimulation>(ps);

    m_running = true;
    QByteArray line;
    DataSet dataset = m_control_algorithm->onCameraResponse(line);

    while(m_running)
    {
        dataset = m_physics_simulation->onModelResponse(dataset);
        line = m_camera_simulator->onSimulatorResponse(dataset);
        dataset = m_control_algorithm->onCameraResponse(line);
    }
    qDebug("Simulation done!");
}

void SimulationRunner::stop()
{
    m_running = false;
}
