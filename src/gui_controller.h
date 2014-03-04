#ifndef GUI_CONTROLLER_H
#define GUI_CONTROLLER_H

#include <QObject>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <mesh_manager.h>
#include <simulation_runner.h>
#include <scene.h>
#include <car3d.h>

class GlobalRenderer;
class SimulationRunner;

class GuiController : public QObject
{
    Q_OBJECT
public:
    GuiController(){}
    GuiController(GlobalRenderer * renderer);
    ~GuiController();

    inline void setOgreEngine(OgreEngine * engine)
    {
        m_ogre_engine=engine;
        m_scene_manager=Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, "SceneManager");
    }

signals:
    void cameraSimulatorCreated(Camera * camera_object);
    void cameraSimulatorDestroyed();

public slots:
    void openTrack();
    void openAlgorithm();
    void queryQuit();
    void runSimulation();
    void pauseSimulation();
    void stopSimulation();
    void viewSwitched(QObject * menu_item);

    void initScene();

private:
    GlobalRenderer * m_global_renderer;
    QQmlContext * m_root_context;
    OgreEngine * m_ogre_engine;
    MeshManager * m_mesh_manager;
    Ogre::SceneManager * m_scene_manager;
    Scene * m_scene;
    Car3D * m_car;
    SimulationRunner * m_simulation_runner;

    QSettings m_settings;
};

#endif
