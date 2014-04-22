#include <scene.h>

Scene::Scene(AssetFactory *asset_factory, QObject * parent)
    : QObject(parent)
    , m_asset_factory(asset_factory)
{
}

Scene::~Scene()
{
    cleanup();
}

bool Scene::loadTrack(const QUrl &file)
{
    using track_library::Tile;

    QString file_path = file.toLocalFile();
    static const QHash<Tile::Type, QString> mesh_mapping =
    {
        { Tile::Crossing, "crossing.mesh" },
        { Tile::Hill,     "bridge.mesh"   },
        { Tile::Line,     "line.mesh"     },
        { Tile::Saw,      "saw.mesh"      },
        { Tile::Start,    "start.mesh"    },
        { Tile::Teeth,    "teeth.mesh"    },
        { Tile::Turn,     "turn.mesh"     }
    };
    constexpr AssetFactory::Flags asset_flags =
            AssetFactory::SceneNode | AssetFactory::Body3D
            | AssetFactory::MeshGeometry;

    cleanup();

    if (!tlio::populateTrackFromFile(m_track, file_path.toStdString()))
    {
        qWarning("Failed to populate track from %s, aborting",
                 file_path.toLocal8Bit().data());
        return false;
    }

    float x, z;
    Ogre::Quaternion q;
    QString mesh_name;

    for (const Tile & tile : m_track.tiles())
    {
        mesh_name = mesh_mapping.value(tile.type(), QString());
        if (mesh_name.isNull())
        {
            qWarning("Failed to populate track from %s, aborting",
                     file_path.toLocal8Bit().data());
            cleanup();
            return false;
        }

        Asset * t = m_asset_factory->createAsset(asset_flags, mesh_name);

        x = -tile.x() + 0.5;
        z = tile.y() + 0.5;
        q = Ogre::Quaternion(Ogre::Degree(tile.rotation()),
                             Ogre::Vector3::UNIT_Y);

        if (tile.type() == track_library::Tile::Start)
            emit startMoved({x, 0, z}, q);

        t->setPosition(x, 0, z);
        t->rotate(q *  Ogre::Quaternion(Ogre::Radian(M_PI), Ogre::Vector3(0, 1, 0)));
        t->setVisible(true);

        dGeomSetCategoryBits(t->geometry, (1 << 0)); // cat 0
        dGeomSetCollideBits(t->geometry, (1 << 1) | (1 << 2)); // collide with 1, 2

        m_tile_assets << t;
    }
    return true;
}

void Scene::cleanup()
{
    m_track.clear();
    qDeleteAll(m_tile_assets);
    m_tile_assets.clear();
}
