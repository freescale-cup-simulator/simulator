#include <car3d.h>

Car3D::Car3D(Ogre::SceneManager * manager,
             Ogre::Vector3 start_pos,
             Ogre::Quaternion start_rot,
             AssetFactory *asset_factory,
             QObject *parent)
    : QObject(parent)
    , m_scene_manager(manager)
    , m_start_pos(start_pos)
    , m_start_rot(start_rot)
    , m_asset_factory(asset_factory)
{
    constexpr dReal spawn_height = 0.1;
    constexpr dReal mass = 1.27;

    Asset * car_body=m_asset_factory->createAsset(
                AssetFactory::SceneNode |
                AssetFactory::Body3D |
                AssetFactory::MeshGeometry |
                AssetFactory::PhyBody,
                "car_body.mesh"
                );
    car_body->setMass(mass);
    car_body->setPosition(m_start_pos.x,
                          m_start_pos.y+spawn_height,
                          m_start_pos.z);
    car_body->rotate(m_start_rot);
}

Car3D::~Car3D()
{
    qDeleteAll(m_car_assets);
    m_car_assets.clear();
}


