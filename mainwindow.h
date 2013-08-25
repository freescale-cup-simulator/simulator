#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QThread>
#include <QMap>
#include <QVariant>
#include <QByteArray>
#include <QPixmap>
#include <QImage>
#include <QString>
#include <QList>
#include <QTimerEvent>
#include <QDoubleSpinBox>

#include <LuaControlAlgorithm.h>
#include <irrlicht/irrlicht.h>
#include <btBulletDynamicsCommon.h>
#include <camerasimulator.h>
#include <track_io.h>
#include <track_model.h>

namespace Ui {
class MainWindow;
}

using namespace irr;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void simulatorResponse(const QMap<QString,QVariant> params);
public slots:
    void onCameraResponse(const QByteArray & frame);
    void onCameraParamsChanged();
    void onControlSignal(const DataSet & data);
private slots:
    void on_pushButton_clicked();

private:
    void timerEvent(QTimerEvent * e);
    Ui::MainWindow *ui;
    CameraSimulator * m_camera;
    QThread * m_thread_cam;
    QThread * m_thread_algo;
    float x,y,z,rx,ry,rz;
    QMap<QString, QVariant> map;
    btDiscreteDynamicsWorld * m_world;
    btRigidBody * m_object;
    LuaControlAlgorithm * m_lua;
    btVector3 m_velocity;
    int m_timer;
    QList<float> m_angle_container;
};

#endif // MAINWINDOW_H
