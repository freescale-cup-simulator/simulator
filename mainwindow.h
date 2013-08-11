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
#include <camerasimulator.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void onCameraResponse(QMap<QString,QVariant> params);
private:
    Ui::MainWindow *ui;
    CameraSimulator * m_camera;
    QThread * m_thread;
};

#endif // MAINWINDOW_H
