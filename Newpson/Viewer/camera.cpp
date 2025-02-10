#include "Newpson/Viewer/camera.h"

#include <QVector3D>
#include <QMatrix4x4>
#include <QPointF>

namespace Newpson {

void Camera::rotate(const QPointF &mouseMotion)
{
    // QMatrix4x4 transform;
    // transform.rotate(-mouseMotion.y(), QVector3D::crossProduct(m_center - m_eye, m_up));
    // transform.rotate(-mouseMotion.x(), QVector3D(m_center.x(), 1.0, m_center.z()));

    // m_up = transform.map(m_up + m_eye);
    // m_eye = transform.map(m_eye);
    // m_up -= m_eye;
    QVector3D viewVector = m_center - m_eye;
    // QVector3D left = QVector3D::normal(viewVector, QVector3D(0.0, 1.0, 0.0));
    QVector3D left = QVector3D::normal(viewVector, QVector3D(0.0, 1.0, 0.0));
    QVector3D up = QVector3D::normal(viewVector, left);
    QVector3D delta = viewVector.length() / 25.0 * ((-mouseMotion.y() / 25) * up + (-mouseMotion.x() / 25) * left);
    m_eye += delta;
    viewVector = m_center - m_eye;
    left = QVector3D::normal(viewVector, QVector3D(0.0, 1.0, 0.0));
    m_up = QVector3D::normal(viewVector, -left);
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
