#ifndef NEWPSON_VIEWER_CAMERA_H
#define NEWPSON_VIEWER_CAMERA_H

#include <QPointF>
#include <QVector3D>
#include <QMatrix4x4>

namespace Newpson::Viewer {

class Camera
{
public:
    Camera() = default;

    void rotate(const QPointF &mouseMotion);
    void slide(const QPointF &mouseMotion);
    void zoom(const float angle8);

    QMatrix4x4 view() const;
    QVector3D direction() const;

private:
    QVector3D m_up = QVector3D(0.0, 1.0, 0.0);
    QVector3D m_center = QVector3D(0.0, 0.0, 0.0);
    QVector3D m_eye = QVector3D(5.0, 0.0, 5.0);
};
    
}

#endif
