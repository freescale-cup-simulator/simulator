#include <QCoreApplication>
#include <QThreadPool>

#include <simulation_runner.h>

int main(int argc, char * argv[])
{
    qRegisterMetaType<DataSet>();

    QCoreApplication application (argc, argv);
    SimulationRunner sr;

    if(sr.loadAlgorithmFile(LUA_DIRECTORY "/simple_algorithm.lua"))
        qDebug("main(): lua algorithm loaded ok");
    sr.loadTrack(RESOURCE_DIRECTORY "tracks/simple-circle.xml");
    QThreadPool::globalInstance()->start(&sr);
    return application.exec();
}
