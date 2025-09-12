#include "sky.hpp"
#include <vector>
#include <GL/glew.h>
#include "arraybuffer.hpp"
#include "program.hpp"

Sky::Sky():
    program(Program::from_shaders(
        Shader::from_file("../shaders/sky.vert", GL_VERTEX_SHADER),
        Shader::from_file("../shaders/sky.frag", GL_FRAGMENT_SHADER))),
    u_camera_pitch(program->get_uniform_location("u_camera_pitch")),
    u_window_size(program->get_uniform_location("u_window_size")),
    array_buffer(ArrayBuffer<Attributes>::create(buffer.size(), buffer.data())),
    a_position(program->get_attribute_location("a_position"))
{
	// FIXME hardcoded constants
    program->set_uniform(u_window_size, glm::vec2(800.0f, 600.0f));
}

void Sky::render() const
{
	glUseProgram(program->id());
	array_buffer->bind();
	glVertexAttribPointer(a_position, glm::vec2::length(), GL_FLOAT,
		GL_FALSE, sizeof(Attributes),
		reinterpret_cast<void *>(offsetof(Attributes, position_x)));
	glEnableVertexAttribArray(a_position);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, buffer.size());
}
