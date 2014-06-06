#ifndef VEHICLE_H
#define VEHICLE_H

#include <QObject>
#include <QHash>

#include <OgreQuaternion.h>

#include <ode/ode.h>

#include <asset_factory.h>
#include <common.h>
#include <util.h>

class Vehicle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal vehicleVelocity READ vehicleVelocity NOTIFY vehicleVelocityChanged)
    Q_PROPERTY(qreal slip1 READ slip1 WRITE setSlip1 NOTIFY slip1Changed)
    Q_PROPERTY(qreal slip2 READ slip2 WRITE setSlip2 NOTIFY slip2Changed)
    Q_PROPERTY(qreal rho READ rho WRITE setRho NOTIFY rhoChanged)
    Q_PROPERTY(qreal mu READ mu WRITE setMu NOTIFY muChanged)

    Q_PROPERTY(qreal suspension_k READ suspension_k WRITE setSuspension_k
               NOTIFY suspension_k_Changed)
    Q_PROPERTY(qreal suspension_damping READ suspension_damping
               WRITE setSuspension_damping NOTIFY suspension_k_Changed)

    Q_PROPERTY(qreal tire_k READ tire_k WRITE setTire_k NOTIFY tire_k_Changed)
    Q_PROPERTY(qreal tire_damping READ tire_damping
               WRITE setTire_damping NOTIFY tire_k_Changed)
public:
    Vehicle(QObject * parent = nullptr);
    ~Vehicle();

    void process(DataSet & dataset);

    inline void setAssetFactory(AssetFactory * af) { m_asset_factory = af; }

    qreal vehicleVelocity() { return m_vehicleVelocity; }
    qreal slip1() const { return m_slip1; }
    qreal slip2() const { return m_slip2; }
    qreal rho() const { return m_rho; }
    qreal suspension_k() const { return m_suspension_k; }
    qreal suspension_damping() const { return m_suspension_damping; }
    qreal tire_k() const { return m_tire_k; }
    qreal tire_damping() const { return m_tire_damping; }
    qreal mu() const { return m_mu; }

    void create();

public slots:
    void startMoved(Ogre::Vector3 pos, Ogre::Quaternion rot);
    void destroy();

    void setSlip1(qreal arg) { m_slip1 = arg; emit slip1Changed(); }
    void setSlip2(qreal arg) { m_slip2 = arg; emit slip2Changed(); }
    void setRho(qreal arg) { m_rho = arg; emit rhoChanged(); }
    void setMu(qreal arg) { m_mu = arg; emit muChanged(); }

    void setSuspension_k(qreal arg)
    {
        m_suspension_k = arg;
        emit suspension_k_Changed();
    }

    void setSuspension_damping(qreal arg)
    {
        m_suspension_damping = arg;
        emit suspension_k_Changed();
    }

    void setTire_k(qreal arg) { m_tire_k = arg; emit tire_k_Changed(); }
    void setTire_damping(qreal arg) { m_tire_damping = arg; emit tire_k_Changed(); }
signals:
    void vehicleVelocityChanged();
    void slip1Changed();
    void slip2Changed();
    void rhoChanged();
    void suspension_k_Changed();
    void tire_k_Changed();
    void muChanged();
private:
    enum Parts {
        PART_CHASIS,
        PART_CAMERA,
        PART_WHEEL_FL, // front left
        PART_WHEEL_FR,
        PART_WHEEL_RL, // rear left
        PART_WHEEL_RR
    };

    void updateERPandCFM(const DataSet &d);
    void updateContact();
    void simulateServo(DataSet &d);

    QHash<int, Asset *> m_parts;
    QHash<int, dJointID> m_joints;
    AssetFactory * m_asset_factory;

    qreal m_vehicleVelocity;
    dReal m_surfaceERP;
    dReal m_surfaceCFM;

    qreal m_slip1;
    qreal m_slip2;
    qreal m_mu;
    qreal m_rho;
    qreal m_suspension_k;
    qreal m_suspension_damping;
    qreal m_tire_k;
    qreal m_tire_damping;

    Ogre::Quaternion m_start_rotation;
    Ogre::Vector3 m_start_position;

    // for Futaba FUTM0032 servo
    static constexpr float DEGREES_PER_SECOND = 60.0 / 0.2;
};

#endif // VEHICLE_H
