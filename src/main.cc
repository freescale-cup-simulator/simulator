#include <QApplication>
#include <QThreadPool>

#include <global_renderer.h>
#include <property.h>
#include <config.h>

int main(int argc, char * argv[])
{
    QCoreApplication::setOrganizationName("WonderCode");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("Freescale Simulator");

    qRegisterMetaType<Property>();
    qRegisterMetaType<DataSet>();

    QApplication application (argc, argv);
    GlobalRenderer view;

    return application.exec();
}
