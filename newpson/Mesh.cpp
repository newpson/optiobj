#include "Mesh.hpp"

namespace Newpson  {

void Mesh::clear()
{
    geometryVertices.clear();
    textureVertices.clear();
    geometryFaces.clear();
    textureFaces.clear();
}

} // namespace Newpson
