#include <QApplication>
#include <QThreadPool>
#include <QOpenGLContext>

#include <global_renderer.h>
#include <ogre_engine.h>
#include <camera_grabber.h>
#include <scene.h>
#include <car3d.h>
#include <asset_manager.h>
#include <simulation_runner.h>
#include <physics_simulation.h>
#include <control_algorithm.h>
#include <config.h>
#include <common.h>

void register_types()
{
    qRegisterMetaType<DataSet>();
    qmlRegisterType<CameraGrabber>("OgreTypes", 1, 0, "CameraGrabber");
    qmlRegisterType<GlobalRenderer>("OgreTypes", 1, 0, "GlobalRenderer");
    qmlRegisterType<OgreEngine>("OgreTypes", 1, 0, "OgreEngine");
    qmlRegisterType<SimulationRunner>("Simulator", 1, 0, "SimulationRuner");
}

int main(int argc, char * argv[])
{
    QCoreApplication::setOrganizationName("WonderCode");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("Freescale Simulator");

    QApplication application (argc, argv);

    register_types();

    GlobalRenderer view;
    SimulationRunner simulation_runner;
    PhysicsSimulation physics;
    TrimeshDataManager trimesh_manager;
    AssetFactory asset_factory(physics.world(), physics.space(), &trimesh_manager);
    Scene scene(&asset_factory);

    QQmlContext * qmlContext = view.rootContext();

    qmlContext->setContextProperty("simulationRunner", &simulation_runner);
    qmlContext->setContextProperty("sceneInstance", &scene);

    view.create();

    GlobalRenderer::RenderingInstances rendering_instances(&view);
    Q_ASSERT(rendering_instances.gl_context->create());

    simulation_runner.setRenderer(&view);
    trimesh_manager.setRenderingInstances(&rendering_instances);
    asset_factory.setRenderingInstances(&rendering_instances);




    //Car3D car();

//    ControlAlgorithm control_algorithm

    //QObject::connect(view.rootWindow(),&QQuickWindow::beforeRendering,&scene,&Scene::update,Qt::DirectConnection);


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
