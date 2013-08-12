#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_thread=new QThread();
    TrackModel model;
    Q_ASSERT(track_library::io::populateTrackFromFile(model,"track/track.xml"));
    m_camera=new CameraSimulator(model);
    m_camera->moveToThread(m_thread);
    connect(m_camera,SIGNAL(cameraResponse(QMap<QString,QVariant>)),this,SLOT(onCameraResponse(QMap<QString,QVariant>)));
    connect(m_thread,SIGNAL(started()),m_camera,SLOT(init()));
    connect(m_camera,SIGNAL(unloaded()),m_thread,SLOT(quit()));
    connect(m_camera,SIGNAL(unloaded()),m_camera,SLOT(deleteLater()));
    connect(this,SIGNAL(simulatorResponse(QMap<QString,QVariant>)),m_camera,SLOT(onSimulatorResponse(QMap<QString,QVariant>)));
    connect(m_thread,SIGNAL(finished()),m_thread,SLOT(deleteLater()));
    m_thread->start();
    foreach (QObject *widget, ui->groupBox->children()) {
        foreach (QObject * child, widget->children()) {
            if(QString(child->metaObject()->className())=="QDoubleSpinBox")
            {
                QDoubleSpinBox * spinBox=reinterpret_cast<QDoubleSpinBox *>(child);
                if(spinBox->objectName().contains("rot"))
                    spinBox->setSingleStep(1);
                else
                    spinBox->setSingleStep(0.01);
                //controls[spinBox->objectName()]=spinBox;
                //oldvalues[spinBox->objectName()]=spinBox->value();
                //qDebug()<<spinBox->objectName();
                connect(spinBox,SIGNAL(valueChanged(double)),this,SLOT(onCameraParamsChanged()));
                //connect(spinBox,SIGNAL(editingFinished()),this,SLOT(onCameraParamsChanged()));
            }
        }
    }
    //Q_DECLARE_METATYPE(core::vector3df);
    int metatype_id = qRegisterMetaType<core::vector3df>("core::vector3df");
    //this->startTimer(2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCameraResponse(QMap<QString, QVariant> params)
{

}

/*
 *-2,38
 *0,58
 *-0,49
 *-56,00
 */

void MainWindow::onCameraParamsChanged()
{
    map.clear();
    map["camX"]=QVariant(ui->poseX->value());
    map["camY"]=QVariant(ui->poseY->value());
    map["camZ"]=QVariant(ui->poseZ->value());
    map["rotateX"]=QVariant(ui->rotX->value());
    map["rotateY"]=QVariant(ui->rotY->value());
    map["rotateZ"]=QVariant(ui->rotZ->value());
    emit simulatorResponse(map);
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    /*x=0;y=2;z=0;
    rx=ry=rz=0;
    map.clear();
    const QList<scene::IAnimatedMeshSceneNode *> * list=m_camera->tiles3d();
    scene::IAnimatedMeshSceneNode * node=list->first();
    map["camX"]=node->getAbsolutePosition().X;
    map["camY"]=node->getAbsolutePosition().Y+2;
    map["camZ"]=node->getAbsolutePosition().Z;
    rx=0;ry=-80;rz=0;
    map["rotateX"]=QVariant(rx);
    map["rotateY"]=QVariant(ry);
    map["rotateZ"]=QVariant(rz);
    emit simulatorResponse(map);
    e->accept();*/
}
