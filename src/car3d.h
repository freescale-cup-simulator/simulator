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
    Car3D();
    ~Car3D();
};

#endif
