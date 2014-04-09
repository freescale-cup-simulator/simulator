#include <scene.h>

Scene::Scene(AssetFactory *asset_factory, QObject * parent)
    : QObject(parent)
    , m_asset_factory(asset_factory)
    , m_start_position(0)
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
    double x=0.0,z=0.0;
    Ogre::Quaternion q;
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
                    AssetFactory::SceneNode |
                    AssetFactory::Body3D |
                    AssetFactory::MeshGeometry,
                    mesh_name);

        x=-tile.x() + 0.5;
        z=tile.y() + 0.5;
        q=Ogre::Quaternion(Ogre::Degree(-tile.rotation() - 180),Ogre::Vector3::UNIT_Y);
        if (tile.type()==track_library::Tile::Start)
        {
            m_start_position=new Ogre::Vector3(x,0.0f,z);
            m_start_rotation_q=q;
        }
        t->setPosition(x, 0, z);
        t->rotate(q);
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
    delete m_start_position;
    m_start_position=0;
}
