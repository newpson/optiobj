#pragma once
#include <memory>
#include <vector>
#include <cstddef>

#include "program.hpp"
#include "transformation.hpp"
#include "renderable.hpp"
#include "vbo.hpp"

using std::shared_ptr;
using std::size_t;
using std::vector;

class Object: public Renderable
{
public:
    Object(const shared_ptr<const Program> &program,
           const vector<shared_ptr<const VertexBufferObject>> &vbos,
           const Transformation &transformation = Transformation());

    void render() override;

protected:
    const shared_ptr<const Program> m_program;
    vector<shared_ptr<const VertexBufferObject>> m_vbos;
    Transformation m_transformation;
    // TODO texture
};
