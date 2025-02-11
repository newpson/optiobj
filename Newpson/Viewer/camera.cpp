#include "Newpson/Viewer/camera.h"

#include <QVector3D>
#include <QMatrix4x4>
#include <QPointF>
#include <cmath>

namespace Newpson {

void addSaveLength(const QVector3D &delta, QVector3D &v)
{
    float vLength = v.length();
    v = (v + delta).normalized() * vLength;
}

void Camera::rotate(const QPointF &mouseMotion)
{
    QVector3D direction = m_center - m_eye;
    QVector3D left = QVector3D::normal(direction, m_up);
    QVector3D delta = direction.length() / 25.0 * ((mouseMotion.y() / 25.0) * m_up + (-mouseMotion.x() / 25.0) * left);
    addSaveLength(delta, m_eye);
    direction = m_center - m_eye;
    left = QVector3D::normal(direction, QVector3D(0.0, 1.0, 0.0));
    m_up = QVector3D::normal(direction, -left);
}

void Camera::slide(const QPointF &mouseMotion)
{
    QVector3D viewVector = m_center - m_eye;
    QVector3D left = QVector3D::normal(viewVector, QVector3D(0.0, 1.0, 0.0));
    QVector3D up = QVector3D::normal(viewVector, left);
    QVector3D delta = viewVector.length() / 25.0 * ((-mouseMotion.y() / 50) * up + (-mouseMotion.x() / 50) * left);
    m_eye += delta;
    m_center += delta;
}

void Camera::zoom(const float angle8)
{
    float delta = std::copysign(0.5, angle8);
    m_eye += delta * (m_center - m_eye) / 10.0;
}

QMatrix4x4 Camera::view() const
{
    QMatrix4x4 transform;
    transform.lookAt(m_eye, m_center, m_up);
    return transform;
}

QVector3D Camera::direction() const
{
    return m_center - m_eye;
}

}
