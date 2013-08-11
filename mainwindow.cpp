#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_thread=new QThread();
    m_camera=new CameraSimulator("track/track.xml",QSize(640,480));
    m_camera->moveToThread(m_thread);
    connect(m_camera,SIGNAL(cameraResponse(QMap<QString,QVariant>)),this,SLOT(onCameraResponse(QMap<QString,QVariant>)));
    connect(m_thread,SIGNAL(started()),m_camera,SLOT(init()));
    connect(m_camera,SIGNAL(unloaded()),m_thread,SLOT(quit()));
    connect(m_camera,SIGNAL(unloaded()),m_camera,SLOT(deleteLater()));
    connect(m_thread,SIGNAL(finished()),m_thread,SLOT(deleteLater()));
    m_thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCameraResponse(QMap<QString, QVariant> params)
{
    qDebug()<<"Frame arrived";
}
