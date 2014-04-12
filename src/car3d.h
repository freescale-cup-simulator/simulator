#ifndef CAR3D_H
#define CAR3D_H

#include <QObject>
#include <QList>
#include <asset_factory.h>

class GlobalRenderer;

class Car3D : public QObject
{
    Q_OBJECT
public:
    Car3D(Ogre::SceneManager * manager,
          Ogre::Vector3 start_pos,
          Ogre::Quaternion start_rot,
          AssetFactory *asset_factory,
          QObject *parent);

    ~Car3D();

private:
    QList<Asset *> m_car_assets;
    Ogre::SceneManager * m_scene_manager;
    Ogre::Vector3 m_start_pos;
    Ogre::Quaternion m_start_rot;
    AssetFactory * m_asset_factory;
};

#endif
