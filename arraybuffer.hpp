#pragma once
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <GL/glew.h>

template <class Attributes>
class ArrayBuffer
{
public:
    static std::shared_ptr<const ArrayBuffer> create(const std::size_t num_attr_sets);
    static std::shared_ptr<const ArrayBuffer> create(
        const std::size_t num_attr_sets, const Attributes *const data);

    ArrayBuffer(const ArrayBuffer &another) = delete;
    ~ArrayBuffer();

    GLuint id() const;
    void bind() const;
    void set_data(const std::size_t num_attr_sets, const Attributes *const data);
protected:
    ArrayBuffer(const std::size_t num_attr_sets, const Attributes *const data);

    GLuint m_id;
};

template <class Attributes>
std::shared_ptr<const ArrayBuffer<Attributes>>
ArrayBuffer<Attributes>::create(const std::size_t num_attr_sets)
{
    return ArrayBuffer::create(num_attr_sets, nullptr);
}

template <class Attributes>
std::shared_ptr<const ArrayBuffer<Attributes>>
ArrayBuffer<Attributes>::create(const std::size_t num_attr_sets, const Attributes *const data)
{
    return std::shared_ptr<const ArrayBuffer<Attributes>>(
        new ArrayBuffer<Attributes>(num_attr_sets, data));
}

template <class Attributes>
ArrayBuffer<Attributes>::~ArrayBuffer()
{
    glDeleteBuffers(1, &m_id);
}

template <class Attributes>
GLuint ArrayBuffer<Attributes>::id() const
{
    return m_id;
}

template <class Attributes>
void ArrayBuffer<Attributes>::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

template <class Attributes>
ArrayBuffer<Attributes>::ArrayBuffer(const std::size_t num_attr_sets,
                                     const Attributes *const data)
{
    glGenBuffers(1, &m_id);
    if (m_id == 0)
        throw std::runtime_error("cannot generate buffer object");
    bind();
    glBufferData(GL_ARRAY_BUFFER, num_attr_sets * sizeof(Attributes), data, GL_STATIC_DRAW);
}
