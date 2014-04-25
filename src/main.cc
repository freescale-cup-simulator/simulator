#include <QApplication>
#include <QOpenGLContext>

#include <global_renderer.h>
#include <ogre_engine.h>
#include <camera_grabber.h>
#include <scene.h>
#include <vehicle.h>
#include <asset_factory.h>
#include <trimesh_data_manager.h>
#include <simulation_runner.h>
#include <physics_simulation.h>
#include <control_algorithm.h>
#include <property.h>
#include <property_model.h>
#include <config.h>
#include <common.h>

#include <user_settings.h>

Q_DECLARE_METATYPE(Ogre::Vector3)
Q_DECLARE_METATYPE(Ogre::Quaternion)

void register_types()
{
    qRegisterMetaType<DataSet>();
    qRegisterMetaType<Property>();
    qRegisterMetaType<Ogre::Vector3>();
    qRegisterMetaType<Ogre::Quaternion>();
    qmlRegisterType<CameraGrabber>("OgreTypes", 1, 0, "CameraGrabber");
    qmlRegisterType<GlobalRenderer>("OgreTypes", 1, 0, "GlobalRenderer");
    qmlRegisterType<OgreEngine>("OgreTypes", 1, 0, "OgreEngine");

    qmlRegisterType<SimulationRunner>("Simulator", 1, 0, "SimulationRuner");
    qmlRegisterType<PhysicsSimulation>("Simulator", 1, 0, "PhysicsSimulation");
    qmlRegisterType<Vehicle>("Simulator", 1, 0, "Vehicle");

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
    UserSettings user_settings;
    PropertyModel property_model;
    Vehicle vehicle;

    QQmlContext * qmlContext = view.rootContext();

    qmlContext->setContextProperty("simulationRunner", &simulation_runner);
    qmlContext->setContextProperty("sceneInstance", &scene);
    qmlContext->setContextProperty("userSettings", &user_settings);
    qmlContext->setContextProperty("physicsSimulation", &physics);
    qmlContext->setContextProperty("vehicle", &vehicle);
    qmlContext->setContextProperty("propertyModel", &property_model);

    view.create();

    GlobalRenderer::RenderingObjects rendering_objects(&view);
    Q_ASSERT(rendering_objects.gl_context->create());

    auto cam = rendering_objects.scene_manager->createCamera("linescanCamera");
    LineScanCamera linescan_camera(cam);

    simulation_runner.setRenderer(&view);

    trimesh_manager.setRenderingObjects(&rendering_objects);
    asset_factory.setRenderingObjects(&rendering_objects);
    linescan_camera.setRenderingObjects(&rendering_objects);

    vehicle.setAssetFactory(&asset_factory);

    simulation_runner.setVehicle(&vehicle);
    simulation_runner.setPhysicsSimulation(&physics);
    simulation_runner.setCameraSimulator(&linescan_camera);

    CameraGrabber * cg;

    cg = view.rootWindow()->findChild<CameraGrabber *>("freeCamera");
    cg->setOgreEngine(view.ogreEngine());
    cg->setCamera(view.userCamera());

    cg = view.rootWindow()->findChild<CameraGrabber *>("linescanCamera");
    cg->setOgreEngine(view.ogreEngine());
    cg->setCamera(&linescan_camera);

    QObject::connect(&scene, &Scene::startMoved, &vehicle, &Vehicle::startMoved);
    QObject::connect(&simulation_runner, &SimulationRunner::simulationStopped,
                     &vehicle, &Vehicle::simulationStopped);

    return application.exec();
}
