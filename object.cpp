#include "object.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "arraybuffer.hpp"
#include "mesh.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "transformation.hpp"

Object::Object(const std::string &path,
               const std::shared_ptr<const Program> &program):
	program(program),
	u_mvp(program->get_uniform_location("u_mvp")),
	u_local_rotation(program->get_uniform_location("u_local_rotation")),
	u_is_selected(program->get_uniform_location("u_is_selected")),
	a_position(program->get_attribute_location("a_position")),
	a_normal(program->get_attribute_location("a_normal")),
	buffer(generate_attribute_sets(load(path).triangulate())),
	array_buffer(ArrayBuffer<Attributes>::create(buffer.size(), buffer.data()))
{}

void Object::render() const
{
	glUseProgram(program->id());
	array_buffer->bind();
	glEnableVertexAttribArray(a_position);
	glEnableVertexAttribArray(a_normal);
	glVertexAttribPointer(a_position, glm::vec3::length(), GL_FLOAT, GL_FALSE,
		sizeof(Attributes), reinterpret_cast<void *>(offsetof(Attributes, position_x)));
	glVertexAttribPointer(a_normal, glm::vec3::length(), GL_FLOAT, GL_FALSE,
		sizeof(Attributes), reinterpret_cast<void *>(offsetof(Attributes, normal_x)));
	glDrawArrays(GL_TRIANGLES, 0, buffer.size());
}

std::vector<Object::Attributes> Object::generate_attribute_sets(const Mesh &mesh)
{
	std::vector<Attributes> attribute_sets;
	const std::size_t num_faces = mesh.refs_ends.size() - 1;
	const std::size_t num_vertices = num_faces * 3;
    attribute_sets.reserve(num_vertices);
	for (std::size_t i = 0; i < num_faces; ++i) {
		for (int ref = mesh.refs_ends[i]; ref < mesh.refs_ends[i+1]; ++ref) {
			const auto &position = mesh.positions[mesh.positions_refs[ref]];
			const auto &normal = mesh.normals[mesh.normals_refs[ref]];
			attribute_sets.push_back({
            	position.x, position.y, position.z,
            	normal.x, normal.y, normal.z
            });
		}
	}
	return attribute_sets;
}
