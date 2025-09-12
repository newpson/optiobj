#pragma once
#include <GL/glew.h>
#include <memory>
#include <vector>
#include "arraybuffer.hpp"
#include "program.hpp"

class Sky
{
public:
	struct Attributes {
		GLfloat position_x;
		GLfloat position_y;
	};
	const std::shared_ptr<const Program> program;
	const GLint u_camera_pitch;
	const GLint u_window_size;

	Sky();
	void render() const;
protected:
    const std::vector<Attributes> buffer = {
        {1.0f, 1.0f},
        {-1.0f, 1.0f},
        {1.0f, -1.0f},
        {-1.0f, -1.0f},
	};
	const std::shared_ptr<const ArrayBuffer<Attributes>> array_buffer;
	const GLint a_position;
};
