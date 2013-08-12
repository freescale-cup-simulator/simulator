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
#include <irrlicht/irrlicht.h>
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
    void onCameraResponse(QMap<QString,QVariant> params);
    void onCameraParamsChanged();
private:
    void timerEvent(QTimerEvent * e);
    Ui::MainWindow *ui;
    CameraSimulator * m_camera;
    QThread * m_thread;
    float x,y,z,rx,ry,rz;
    QMap<QString, QVariant> map;
};

#endif // MAINWINDOW_H
