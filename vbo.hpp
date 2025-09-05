#pragma once
#include <memory>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::shared_ptr;
using std::vector;
using std::string;

class VertexBufferObject
{
public:
    static shared_ptr<const VertexBufferObject> create(const string &name, const vector<GLfloat> &raw_data, const GLsizei num_indices);

    VertexBufferObject(const VertexBufferObject &another) = delete;
    ~VertexBufferObject();

    GLuint id() const;
    GLsizei num_indices() const;
    string name() const;

protected:
    VertexBufferObject(const string &name, const vector<GLfloat> &raw_data, const GLsizei num_indices);

    GLuint m_id;
    GLsizei m_num_indices;
    string m_name;
};
