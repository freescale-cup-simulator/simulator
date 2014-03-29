#ifndef CAR3D_H
#define CAR3D_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <global_renderer.h>
#include <common.h>

#include <OGRE/OgreEntity.h>

class GlobalRenderer;

class Car3D : public QObject
{
    Q_OBJECT

public:
    Car3D(GlobalRenderer * renderer);
    ~Car3D();
    void process(DataSet & d);
public slots:
    void setVisible(bool visible);
    void update();

private:
    OgreEngine * m_engine;
    Ogre::SceneManager * m_scene_manager;
    Ogre::SceneNode * m_car_body;
    Ogre::SceneNode * m_wheels[4];
    DataSet m_local_dataset;
    QMutex m_local_dataset_locker,m_safe_destruct;
    GlobalRenderer * m_renderer;
};

#endif
