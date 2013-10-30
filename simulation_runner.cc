#include <simulation_runner.h>

SimulationRunner::SimulationRunner(GlobalRenderer * renderer,QObject *parent)
    : QObject(parent)
    , m_running(false)
    , m_control_interval(0.1)
    , m_physics_timestep(8e-3)
    , m_renderer(renderer)
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

TrackModel *SimulationRunner::loadTrack(const QString &track_path)
{
    if (m_running)
    {
        qWarning("Will not change track while running");
        return 0;
    }

    m_track_model.clear();
    bool s = track_library::io::populateTrackFromFile(m_track_model,
                                                      track_path.toStdString());
    if (!s)
    {
        qWarning("Failed to populate track from %s, aborting",
                 track_path.toLocal8Bit().data());
        return 0;
    }
    return &m_track_model;
}

void SimulationRunner::run()
{
    if (!m_control_algorithm)
    {
        qWarning("Control algorithm not loaded, will not run");
        return;
    }
    CameraGrabber * camGrab=m_renderer->createCameraGrabber(&m_cameras_syncronizator);
    Camera * cameraObj=qobject_cast<Camera *>(camGrab->camera());
    SharedImage * shBuffer=qobject_cast<SharedImage *>(camGrab->sharedImage());

    auto cs = new CameraSimulator(cameraObj->get(),shBuffer);
    auto ps = new PhysicsSimulation(m_track_model);
    auto vm = new VehicleModel;
    m_camera_simulator = QSharedPointer<CameraSimulator>(cs);
    m_physics_simulation = QSharedPointer<PhysicsSimulation>(ps);
    m_vehicle_model = QSharedPointer<VehicleModel>(vm);

    m_running = true;
    DataSet dataset;
    float physics_runtime;

    dataset.current_wheel_angle = 0;
    dataset.physics_timestep = m_physics_timestep;
    dataset.control_interval = m_control_interval;
    m_control_algorithm->process(dataset);

    m_cameras_syncronizator.acquire(VIRTUAL_CAMERAS_COUNT);

    while (m_running)
    {
        physics_runtime = 0;
        //while (physics_runtime < m_control_interval)
        //{
            m_vehicle_model->process(dataset);
            m_physics_simulation->process(dataset);
            physics_runtime += m_physics_timestep;
        //}
        m_camera_simulator->process(dataset);
        m_renderer->process(dataset);
        m_control_algorithm->process(dataset);
    }
    qDebug("Simulation done!");
}

void SimulationRunner::stop()
{
    m_running = false;
}
