#ifndef UTIL_H
#define UTIL_H

#include <cmath>

#include <ode/ode.h>

#include <QVector3D>
#include <QQuaternion>
#include <OgreVector3.h>
#include <OgreQuaternion.h>

namespace util
{

inline QVector3D dv2qv (const dReal * dv)
{
    return QVector3D (dv[0], dv[1], dv[2]);
}

inline QQuaternion dq2qq(const dReal * q)
{
    return QQuaternion (q[0], q[1], q[2], q[3]);
}

inline Ogre::Quaternion dq2oq(const dReal * q)
{
    return Ogre::Quaternion(q[0], q[1], q[2], q[3]);
}

inline qreal anglev (const QVector3D & a, const QVector3D & b)
{
    const qreal v = QVector3D::dotProduct(a, b) / (a.length() * b.length());
    return (std::acos(v) / M_PI) * 180.0;
}

}

#endif // UTIL_H
