#ifndef GUI_CONTROLLER_H
#define GUI_CONTROLLER_H

#include <QObject>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <simulation_runner.h>
#include <car3d.h>

class GlobalRenderer;
class SimulationRunner;

class GuiController : public QObject
{
    Q_OBJECT
public:
    GuiController(){}
    GuiController(GlobalRenderer * renderer);

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
    GlobalRenderer * m_renderer;
    SimulationRunner * m_simulation_runner;
    track_library::TrackModel * m_track;
    QSettings m_settings;
    Car3D * m_car;
};

#endif
