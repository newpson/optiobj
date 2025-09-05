#include "vbo.hpp"
#include <memory>
#include <vector>
#include <stdexcept>
#include <string>

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::shared_ptr;
using std::vector;
using std::runtime_error;
using std::string;

shared_ptr<const VertexBufferObject> VertexBufferObject::create(const string &name, const vector<GLfloat> &raw_data, const GLsizei num_indices)
{
    return shared_ptr<const VertexBufferObject>(new VertexBufferObject(name, raw_data, num_indices));
}

VertexBufferObject::~VertexBufferObject()
{
    std::cerr << "[DEBUG] deleting buffer #" << m_id << std::endl;
    glDeleteBuffers(1, &m_id);
}

GLuint VertexBufferObject::id() const
{
    return m_id;
}

GLsizei VertexBufferObject::num_indices() const
{
    return m_num_indices;
}

string VertexBufferObject::name() const
{
    return m_name;
}

VertexBufferObject::VertexBufferObject(const string &name, const vector<GLfloat> &raw_data, const GLsizei num_indices)
    : m_name(name), m_num_indices(num_indices)
{
    glGenBuffers(1, &m_id);
    if (m_id == 0)
        throw runtime_error("cannot create VBO handle");
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, raw_data.size() * sizeof(GLfloat), raw_data.data(), GL_STATIC_DRAW);
}
