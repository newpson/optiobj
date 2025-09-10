#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "program.hpp"
#include "shader.hpp"
#include "arraybuffer.hpp"
#include "inputprovider.hpp"
#include "mesh.hpp"
#include "parser.hpp"

#include "camera.hpp"
// FIXME
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void print_error()
{
	switch (glGetError()) {
	case GL_NO_ERROR: std::cout << "no error" << std::endl; break;
	case GL_INVALID_ENUM: std::cout << "invalid enum" << std::endl; break;
	case GL_INVALID_VALUE: std::cout << "invalid value" << std::endl; break;
	case GL_INVALID_OPERATION: std::cout << "invalid operation" << std::endl; break;
	case GL_INVALID_FRAMEBUFFER_OPERATION: std::cout << "invalid framebuffer operation" << std::endl; break;
	case GL_OUT_OF_MEMORY: std::cout << "out of memory" << std::endl; break;
	default: std::cout << "broken" << std::endl; break;
	}
}

class SkyUpdater: public InputProvider::MouseListener
{
	glm::vec2 last_values;
	GLint u_camera_pitch;
	std::shared_ptr<const Program> program;
	float pitch = 0.0;

public:
	SkyUpdater(std::shared_ptr<const Program> program): program(program)
	{
		u_camera_pitch = program->get_uniform_location("u_camera_pitch");
	}

	virtual void on_mouse_move(glm::vec2 values) override
	{
		const glm::vec2 delta = values - last_values;
		// std::cout << "moving mouse: " << delta.x << " " << delta.y << std::endl;
		pitch += 0.01 * delta.y;
		if (pitch > 3.1415/2.0)
			pitch = 3.1415/2.0;
		if (pitch < -3.1415/2.0)
			pitch = -3.1415/2.0;
	    program->set_uniform(u_camera_pitch, pitch);
		last_values = values;
	}
	virtual void on_mouse_button(int button, int action, int mods) override {}
	virtual void on_mouse_scroll(glm::vec2 values) override {}
};

class Sky
{
public:
	struct Attributes {
		GLfloat position_x;
		GLfloat position_y;

		static constexpr size_t num_floats() { return sizeof(Attributes)/sizeof(GLfloat); }
	};

	Sky(): array_buffer(ArrayBuffer<Attributes>::create(buffer.size(), buffer.data())),
		   program(Program::from_shaders(
	           Shader::from_file("../shaders/sky.vert", GL_VERTEX_SHADER),
	           Shader::from_file("../shaders/sky.frag", GL_FRAGMENT_SHADER))),
		   u_window_size(program->get_uniform_location("u_window_size")),
		   u_camera_pitch(program->get_uniform_location("u_camera_pitch")),
		   a_position(program->get_attribute_location("a_position"))
	{
		std::cout << "Sky values:\n"
				  << "buffer attr sets = " << buffer.size() << "\n"
				  << "array_buffer = " << array_buffer->id() << "\n"
				  << "program = " << program->id() << "\n"
				  << "u_window_size = " << u_window_size << "\n"
				  << "u_camera_pitch = " << u_camera_pitch << "\n"
				  << "a_position = " << a_position << "\n"
				  << std::endl;
		array_buffer->bind();
		// FIXME hardcoded constants
	    program->set_uniform(u_window_size, glm::vec2(800.0f, 600.0f));
	}

	void render()
	{
		glUseProgram(program->id());
		array_buffer->bind();
		glVertexAttribPointer(a_position, glm::vec2::length(), GL_FLOAT,
			GL_FALSE, sizeof(Attributes),
			reinterpret_cast<void *>(offsetof(Attributes, position_x)));
		glEnableVertexAttribArray(a_position);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, buffer.size());
	}
protected:
    const std::vector<Attributes> buffer = {
        {1.0f, 1.0f},
        {-1.0f, 1.0f},
        {1.0f, -1.0f},
        {-1.0f, -1.0f},
	};
	const std::shared_ptr<const ArrayBuffer<Attributes>> array_buffer;
	const std::shared_ptr<const Program> program;
	const GLint u_window_size;
	const GLint u_camera_pitch;
	const GLint a_position;
};

class Object
{
public:
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

	Object(): buffer(generate_attribute_sets(load("../assets/suzanne.obj").triangulate())),
			  array_buffer(ArrayBuffer<Attributes>::create(buffer.size(), buffer.data())),
			  program(Program::from_shaders(
	              Shader::from_file("../shaders/object.vert", GL_VERTEX_SHADER),
	              Shader::from_file("../shaders/object.frag", GL_FRAGMENT_SHADER))),
		      u_mvp(program->get_uniform_location("u_mvp")),
		      a_position(program->get_attribute_location("a_position")),
		      a_normal(program->get_attribute_location("a_normal"))
	{
		// glBindAttribLocation(program->id(), a_position, "a_position");
		std::cout << "Object values:\n"
				  << "buffer attr sets = " << buffer.size() << "\n"
				  << "array_buffer = " << array_buffer->id() << "\n"
				  << "program = " << program->id() << "\n"
				  << "u_mvp = " << u_mvp << "\n"
				  << "a_position = " << a_position << "\n"
				  << "a_normal = " << a_normal << "\n"
				  << std::endl;
		array_buffer->bind();
		// GLint _array_buffer_binding;
		// GLint _current_program;
		// glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &_array_buffer_binding);
		// glGetIntegerv(GL_CURRENT_PROGRAM, &_current_program);
		// std::cout << "Object glGet values:\n"
		// 		  << "GL_ARRAY_BUFFER_BINDING = " << _array_buffer_binding << "\n"
		// 		  << "GL_CURRENT_PROGRAM = " << _current_program << "\n"
		// 		  << std::endl;

		Camera camera;
		camera.translate({0.0f, 0.0f, 2.0f});
	    program->set_uniform(u_mvp, camera.perspective() * camera.matrix());
	}

	void render()
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

protected:
	static std::vector<Attributes> generate_attribute_sets(const Mesh &mesh)
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

    const std::vector<Attributes> buffer;
	const std::shared_ptr<const ArrayBuffer<Attributes>> array_buffer;
	const std::shared_ptr<const Program> program;
	const GLint u_mvp;
	const GLint a_position;
	// const GLint a_position = 2;
	const GLint a_normal;
};

int main()
{
    if (glfwInit() == GL_FALSE)
		throw std::runtime_error("cannot initialize glfw");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow *window = glfwCreateWindow(800, 600, "optiobj", nullptr, nullptr);
	if (window == nullptr)
		throw std::runtime_error("cannot create window");
    glfwMakeContextCurrent(window);
    // InputProvider::bind_callbacks(window);
    glfwSwapInterval(1);
	if (glewInit() != GLEW_OK)
        throw std::runtime_error("cannot initialize glew");

    Sky sky;
    // SkyUpdater updater(program_sky);

 	Object object;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// draw sky
		glDisable(GL_DEPTH_TEST);
		sky.render();

		// draw objects
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		object.render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    std::cout << "kill me please" << std::endl;
    return 0;
}
