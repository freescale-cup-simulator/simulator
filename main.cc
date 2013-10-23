#include <QtGui/QGuiApplication>
#include <QThreadPool>

#include <track_model.h>
#include <simulation_runner.h>
#include <global_renderer.h>


int main(int argc, char * argv[])
{
    qRegisterMetaType<DataSet>();

    QGuiApplication application (argc, argv);
    SimulationRunner sr;

    if(sr.loadAlgorithmFile(LUA_DIRECTORY "/simple_algorithm.lua"))
        qDebug("main(): lua algorithm loaded ok");
    track_library::TrackModel * track=sr.loadTrack(RESOURCE_DIRECTORY "tracks/simple-circle.xml");
    //QThreadPool::globalInstance()->start(&sr);

    GlobalRenderer view(track);
    view.resize(800,600);
    view.show();
    view.raise();

    return application.exec();
}
