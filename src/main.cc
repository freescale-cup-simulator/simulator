#include <QApplication>
#include <QThreadPool>

#include <global_renderer.h>
#include <ogre_engine.h>
#include <camera_grabber.h>
#include <scene.h>
#include <car3d.h>
#include <asset_manager.h>
#include <simulation_runner.h>
#include <physics_simulation.h>
#include <config.h>
#include <common.h>

int main(int argc, char * argv[])
{
    QCoreApplication::setOrganizationName("WonderCode");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("Freescale Simulator");

    qRegisterMetaType<DataSet>();
    qmlRegisterType<CameraGrabber>("OgreTypes", 1, 0, "CameraGrabber");
    qmlRegisterType<GlobalRenderer>("OgreTypes", 1, 0, "GlobalRenderer");
    qmlRegisterType<OgreEngine>("OgreTypes", 1, 0, "OgreEngine");

    QApplication application (argc, argv);
    GlobalRenderer view; // ok
    QQmlContext * qmlContext=view.rootContext();
    OgreEngine * engine=view.ogreEngine();
    PhysicsSimulation physics; // need cleanup
    AssetManager asset_manager(physics.world(),physics.space(),engine->sceneManager()); // ok
    Scene scene(engine->sceneManager());
    //Car3D car();
    SimulationRunner simulation_runner(&view);


    /*SimulationRunner sr(&view);

    if(sr.loadAlgorithmFile(LUA_DIRECTORY "/simple_algorithm.lua"))
        qDebug("main(): lua algorithm loaded ok");
    track_library::TrackModel * track=sr.loadTrack(RESOURCE_DIRECTORY "tracks/simple-circle.xml");

    view.setTrackModel(track);

    SimulationRunner * sr_ptr=&sr;
    application.connect(&view, &GlobalRenderer::startSimulation, [sr_ptr]() {
        qDebug()<<"Starting simulation...";
        QThreadPool::globalInstance()->start(sr_ptr);
    });*/
    return application.exec();
}
