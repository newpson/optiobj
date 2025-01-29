#include <QVector>
#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QDebug>
#include <QFile>

class Mesh
{
public:
    QVector<QVector3D> geometryVertices;
    QVector<QVector<int>> geometryFaces;
    QVector<QVector2D> textureVertices;
    QVector<QVector<int>> textureFaces;

public:
    Mesh(QFile &&file)
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream input(&file);
        while (!input.atEnd()) {
            QString line = input.readLine();
            if (line.isEmpty() || line[0] == '#')
                continue;

            QStringList tokens = line.split(" ");
            QString &type = tokens[0];
            if (type == "v") {
                float x = tokens[1].toFloat();
                float y = tokens[2].toFloat();
                float z = tokens[3].toFloat();
                geometryVertices.append(QVector3D(x, y, z));
            } else if (type == "vt") {
                float x = tokens[1].toFloat();
                float y = tokens[2].toFloat();
                textureVertices.append(QVector2D(x, y));
            } else if (type == "f") {
                QStringList v1 = tokens[1].split("/");
                QStringList v2 = tokens[2].split("/");
                QStringList v3 = tokens[3].split("/");
                if (tokens.length() > 4) {
                    QStringList v4 = tokens[4].split("/");
                    geometryFaces.append({v1[0].toInt(), v2[0].toInt(), v3[0].toInt(), v4[0].toInt()});
                    if (!v1[1].isEmpty())
                        textureFaces.append({v1[1].toInt(), v2[1].toInt(), v3[1].toInt(), v4[1].toInt()});
                } else {
                    geometryFaces.append({v1[0].toInt(), v2[0].toInt(), v3[0].toInt()});
                    if (!v1[1].isEmpty())
                        textureFaces.append({v1[1].toInt(), v2[1].toInt(), v3[1].toInt()});
                }


            }
        }
    }

    Mesh(QString const &filename):
        Mesh(QFile(filename))
    {}
};

int main()
{
    Mesh mesh("assets/cube.obj");

    qDebug() << "Geometry vertices";
    for (QVector3D &vertex: mesh.geometryVertices)
        qDebug() << vertex;

    qDebug() << "Texture vertices";
    for (QVector2D &vertex: mesh.textureVertices)
        qDebug() << vertex;

    qDebug() << "Geometry faces";
    for (auto &face: mesh.geometryFaces)
        qDebug() << face;

    qDebug() << "Texture faces";
    for (auto &face: mesh.textureFaces)
        qDebug() << face;

    return 0;
}
