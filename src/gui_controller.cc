#include <gui_controller.h>

GuiController::GuiController(GlobalRenderer *renderer)
    : m_renderer(renderer)
    , m_simulation_runner(new SimulationRunner(renderer,this))
    , m_track(0)
    , m_car(0)
{

}

void GuiController::openTrack(QString filename)
{
    if (filename.isNull())
        filename = QFileDialog::getOpenFileName(nullptr,
                                                tr("Open track file..."),
                                                QString(),
                                                tr("Track Files (*.xml)"));
    if (filename.isEmpty())
        return;

    m_track = m_simulation_runner->loadTrack(filename);
    if (m_track == nullptr)
        QMessageBox::warning(nullptr, tr("Track loading error"),
                             tr("Can't load track file."));

    m_settings.setValue("LastOpenTrack", filename);

    connect(m_renderer->getQuickWindow(), &QQuickWindow::beforeRendering,
            this, &GuiController::initScene, Qt::DirectConnection);
}

void GuiController::openAlgorithm(QString filename)
{
    if (filename.isNull())
        filename = QFileDialog::getOpenFileName(nullptr,
                                                tr("Open algorithm file..."),
                                                QString(),
                                                tr("Lua Script Files (*.lua)"));
    if (filename.isEmpty())
        return;

    m_settings.setValue("LastOpenAlgorithm", filename);

    if (!m_simulation_runner->loadAlgorithmFile(filename))
        QMessageBox::warning(nullptr, tr("Lua script loading error"),
                             tr("Can't load algorithm file."));
}

void GuiController::quickStart()
{
    openAlgorithm(m_settings.value("LastOpenAlgorithm", QString()).toString());
    openTrack(m_settings.value("LastOpenTrack", QString()).toString());
}

void GuiController::queryQuit()
{

}

void GuiController::runSimulation()
{

    if (!m_car)
        return;
    if (m_simulation_runner->getState() != SimulationRunner::Stopped)
        return;
    m_simulation_runner->setCar(m_car);
    m_car->setVisible(true);
    connect(m_renderer->getQuickWindow(), &QQuickWindow::beforeRendering,
            m_car, &Car3D::update, Qt::DirectConnection);
    QThreadPool::globalInstance()->start(m_simulation_runner);
}

void GuiController::pauseSimulation()
{
    switch (m_simulation_runner->getState())
    {
    case SimulationRunner::Started:
        m_simulation_runner->pause();
        break;
    case SimulationRunner::Paused:
        m_simulation_runner->resume();
        break;
    }
}

void GuiController::stopSimulation()
{
    m_simulation_runner->stop();
}

void GuiController::viewSwitched(QObject *menu_item)
{
    qDebug()<<menu_item->objectName();
}

void GuiController::initScene()
{
    disconnect(m_renderer->getQuickWindow(), &QQuickWindow::beforeRendering,
               this, &GuiController::initScene);
    if (m_car)
        delete m_car;
    m_renderer->getOgreEngine()->activateOgreContext();
    Ogre::SceneManager * scene_manager = m_renderer->getSceneManager();
    scene_manager->getRootSceneNode()->removeAndDestroyAllChildren();
    scene_manager->destroyAllEntities();
    scene_manager->destroyAllMovableObjects();

    for (const Tile & tile : m_track->tiles())
    {
        Ogre::Entity* current = 0;
        switch(tile.type())
        {
        case Tile::Line:
            current = scene_manager->createEntity("line.mesh");
            break;
        case Tile::Crossing:
            current = scene_manager->createEntity("crossing.mesh");
            break;
        case Tile::Start:
            current = scene_manager->createEntity("start.mesh");
            break;
        case Tile::Saw:
            current = scene_manager->createEntity("saw.mesh");
            break;
        case Tile::Turn:
            current = scene_manager->createEntity("turn.mesh");
            break;
        default:
            current = scene_manager->createEntity("crossing.mesh");
        }

        Ogre::SceneNode * node
                = scene_manager->getRootSceneNode()->createChildSceneNode();
        node->attachObject(current);
        node->setPosition(-tile.x() + 0.5, 0, tile.y() + 0.5);
        node->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-tile.rotation() - 180));
    }
    m_renderer->getOgreEngine()->doneOgreContext();
    m_car = new Car3D(m_renderer);
}
