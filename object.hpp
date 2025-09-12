#pragma once
#include <memory>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "arraybuffer.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include "transformation.hpp"

class Object
{
public:
	const std::shared_ptr<const Program> program;
	const GLint u_mvp;
	const GLint u_local_rotation;
	const GLint u_is_selected;
	Transformation transformation;

	struct Attributes {
		GLfloat position_x;
		GLfloat position_y;
		GLfloat position_z;
		GLfloat normal_x;
		GLfloat normal_y;
		GLfloat normal_z;
		// float texture_x;
		// float texture_y;
	};

	Object(const std::string &path, const std::shared_ptr<const Program> &program);
	void render() const;

protected:
	static std::vector<Attributes> generate_attribute_sets(const Mesh &mesh);

	const GLint a_position;
	const GLint a_normal;
    const std::vector<Attributes> buffer;
	const std::shared_ptr<const ArrayBuffer<Attributes>> array_buffer;
};
