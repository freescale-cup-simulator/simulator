#include <scene.h>

Scene::Scene(Ogre::SceneManager * manager, AssetFactory *asset_factory, QObject *parent)
    : m_scene_manager(manager)
    , m_asset_factory(asset_factory)
    , m_is_dirty(false)
{

}

Scene::~Scene()
{
    cleanup();
}

bool Scene::loadTrack(const QUrl &file)
{
    m_file_url=file;
    QString file_path=m_file_url.toLocalFile();
    using track_library::Tile;

    cleanup();

    if (!tlio::populateTrackFromFile(m_track,
                                     file_path.toStdString()))
    {
        qWarning("Failed to populate track from %s, aborting",
                 file_path.toLocal8Bit().data());
        return false;
    }
    for (const Tile & tile : m_track.tiles())
    {
        QString mesh_name;
        switch(tile.type())
        {
        case Tile::Crossing:
            mesh_name="crossing.mesh";
            break;
        case Tile::Hill:
            mesh_name="bridge.mesh";
            break;
        case Tile::Line:
            mesh_name="line.mesh";
            break;
        case Tile::Saw:
            mesh_name="saw.mesh";
            break;
        case Tile::Start:
            mesh_name="start.mesh";
            break;
        case Tile::Teeth:
            mesh_name="teeth.mesh";
            break;
        case Tile::Turn:
            mesh_name="turn.mesh";
            break;
        default:
            qWarning("Failed to populate track from %s, aborting",
                     file_path.toLocal8Bit().data());
            return false;
        }

        Asset * t=m_asset_factory->createAsset(
                    AssetFactory::Body3D|AssetFactory::MeshGeometry,
                    mesh_name);
        t->setPosition(-tile.x() + 0.5, 0, tile.y() + 0.5);
        t->rotate(Ogre::Quaternion(Ogre::Degree(-tile.rotation() - 180),Ogre::Vector3::UNIT_Y));
        t->setVisible(true);
        m_tile_assets<<t;
    }
    //m_is_dirty=true;
    return true;
}

void Scene::cleanup()
{
    m_track.clear();
    qDeleteAll(m_tile_assets);
    m_tile_assets.clear();
}
