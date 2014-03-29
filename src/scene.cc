#include <scene.h>

Scene::Scene(Ogre::SceneManager * manager,QObject *parent)
    : m_scene_manager(manager)
{

}

bool Scene::loadTrack(const QString &file)
{
    m_track.clear();
    if(track_library::io::populateTrackFromFile(m_track,
                                                file.toStdString()))
    {
        qWarning("Failed to populate track from %s, aborting",
                 file.toLocal8Bit().data());
        return false;
    }

}

void Scene::redraw()
{

}
