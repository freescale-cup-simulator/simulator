#include <QtGui/QGuiApplication>
#include <QThreadPool>

#include <track_model.h>
#include <simulation_runner.h>
#include <global_renderer.h>
#include <config.h>

int main(int argc, char * argv[])
{
    qRegisterMetaType<DataSet>();

    QGuiApplication application (argc, argv);
    GlobalRenderer view;
    SimulationRunner sr(&view);

    if(sr.loadAlgorithmFile(LUA_DIRECTORY "/simple_algorithm.lua"))
        qDebug("main(): lua algorithm loaded ok");
    track_library::TrackModel * track=sr.loadTrack(RESOURCE_DIRECTORY "tracks/simple-circle.xml");

    view.setTrackModel(track);

    SimulationRunner * sr_ptr=&sr;
    application.connect(&view, &GlobalRenderer::startSimulation, [sr_ptr]() {
        qDebug()<<"Starting simulation...";
        QThreadPool::globalInstance()->start(sr_ptr);
    });
    return application.exec();
}
