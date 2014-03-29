#include <car3d.h>

Car3D::Car3D(GlobalRenderer *renderer)
    : m_engine(renderer->ogreEngine())
    , m_scene_manager(m_engine->sceneManager())
    , m_car_body(0)
    , m_renderer(renderer)
{
    m_engine->activateOgreContext();
    Ogre::Entity * ent;
    ent = m_scene_manager->createEntity("car", "car_body.mesh");
    m_car_body = m_scene_manager->getRootSceneNode()->createChildSceneNode();
    m_car_body->attachObject(ent);
    m_car_body->setVisible(false);
    for (int i = 0; i < 4; i++)
    {
        m_wheels[i] = m_scene_manager->getRootSceneNode()->createChildSceneNode();
        ent = m_scene_manager->createEntity((i < 2) ? "wheel_h.mesh"
                                                    : "wheel_r.mesh");
        m_wheels[i]->attachObject(ent);
        m_wheels[i]->setVisible(false);
    }

    m_engine->doneOgreContext();
}

Car3D::~Car3D()
{
    QMutexLocker locker(&m_safe_destruct);
    disconnect(m_renderer->rootWindow(),&QQuickWindow::beforeRendering,this,&Car3D::update);
}

void Car3D::process(DataSet &d)
{
    m_local_dataset_locker.tryLock();
    m_local_dataset=d;
    m_local_dataset_locker.unlock();
}

void Car3D::setVisible(bool visible)
{
    if(!m_car_body)
        return;
    m_car_body->setVisible(visible);
    for(int i=0;i<4;i++)
        m_wheels[i]->setVisible(visible);
}

void Car3D::update()
{
    if(!m_safe_destruct.tryLock())
        return;
    m_local_dataset_locker.lock();
    m_engine->activateOgreContext();

    m_car_body->setPosition(m_local_dataset.vehicle.p.x(), m_local_dataset.vehicle.p.y(),
                            m_local_dataset.vehicle.p.z());
    Ogre::Quaternion q(m_local_dataset.vehicle.q.scalar(), m_local_dataset.vehicle.q.x(),
                       m_local_dataset.vehicle.q.y(), m_local_dataset.vehicle.q.z());
    m_car_body->setOrientation(q);
    m_car_body->_updateBounds();

    for (int i = 0; i < 4; i++)
    {
        m_wheels[i]->setPosition(m_local_dataset.wheels[i].p.x(), m_local_dataset.wheels[i].p.y(),
                                 m_local_dataset.wheels[i].p.z());
        Ogre::Quaternion q(m_local_dataset.wheels[i].q.scalar(), m_local_dataset.wheels[i].q.x(),
                           m_local_dataset.wheels[i].q.y(), m_local_dataset.wheels[i].q.z());

        // we use same models for left/right side, so rotate wheels on the right
        // side to face outwards
        if (i % 2 == 0)
            q = q * Ogre::Quaternion(Ogre::Radian(M_PI), Ogre::Vector3(0, 1, 0));

        m_wheels[i]->setOrientation(q);
        m_wheels[i]->_updateBounds();
    }
    m_engine->doneOgreContext();
    m_local_dataset_locker.unlock();
    m_safe_destruct.unlock();
}
