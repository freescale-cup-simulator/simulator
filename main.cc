#include <QCoreApplication>
#include <QtDebug>
#include <QTime>
#include <QByteArray>

#include <control_algorithm.h>
#include <physics_simulation.h>
#include <camera_simulator.h>
#include <track_io.h>
#include <config.h>

constexpr int LINE_WIDTH = 5;
constexpr int BLACK_VALUE = 15;
constexpr int WHITE_VALUE = 75;
constexpr int FRAME_SIZE = 128;

QByteArray generateFrame(int lp)
{
    QByteArray f (128, 0);

    for (int i = 0; i < lp; i++)
        f[i] = WHITE_VALUE * (0.95 + (qrand() % 30) / 100.0);
    if (lp + LINE_WIDTH >= 128)
        lp = 128 - LINE_WIDTH;
    for (int i = lp; i < lp + LINE_WIDTH; i++)
        f[i] = BLACK_VALUE * (0.95 + (qrand() % 30) / 100.0);
    for (int i = lp + LINE_WIDTH; i < 128; i++)
        f[i] = WHITE_VALUE * (0.95 + (qrand() % 30) / 100.0);
    return f;
}

int main(int argc, char * argv[])
{
    QCoreApplication application (argc, argv);

    track_library::TrackModel m;
    track_library::io::populateTrackFromFile(m, RESOURCE_DIRECTORY "/tracks/simple-circle.xml");

    DataSet empty;
    PhysicsSimulation s(m);
    CameraSimulator cs(m);
    ControlAlgorithm ca;


    qRegisterMetaType<DataSet>();
    ca.loadFile(LUA_DIRECTORY "/simple_algorithm.lua");

    QObject::connect(&s, &PhysicsSimulation::simulationResponse,
                     &cs, &CameraSimulator::onSimulatorResponse,
                     Qt::QueuedConnection);

    QObject::connect(&cs, &CameraSimulator::cameraResponse,
                     &ca, &ControlAlgorithm::onCameraResponse,
                     Qt::QueuedConnection);

    QObject::connect(&ca, &ControlAlgorithm::controlSignal,
                     &s, &PhysicsSimulation::onModelResponse,
                     Qt::QueuedConnection);

//    QObject::connect(&cs, &CameraSimulator::cameraResponse, [](const QByteArray & f)
//    {
//        qDebug() << "got frame\n";

//    });

    s.initiateSimulation();

//    for (int i = 0; i < 400; i++)
//    s.onModelResponse(empty);

//    ControlAlgorithm l;

//    qsrand(QTime::currentTime().msec());

//    l.loadFile("../lua-test/lua/simple_algorithm.lua");

//    QObject::connect(&l, &ControlAlgorithm::controlSignal, [](const DataSet & data)
//    {
//        qDebug() << "Have controlSignal with angle =" << data["angle"].toFloat()
//                 << ", rspeed =" << data["rspeed"].toFloat();
//    });

//    for (int i = 0; i < 32; i++)
//        l.onCameraResponse(generateFrame(i * 4));

    return application.exec();
}
