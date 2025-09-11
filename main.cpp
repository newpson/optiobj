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

// class SkyUpdater: public InputProvider::MouseListener
// {
// 	glm::vec2 last_values;
// 	GLint u_camera_pitch;
// 	std::shared_ptr<const Program> program;
// 	float pitch = 0.0;

// public:
// 	SkyUpdater(std::shared_ptr<const Program> program): program(program)
// 	{
// 		u_camera_pitch = program->get_uniform_location("u_camera_pitch");
// 	}

// 	virtual void on_mouse_move(glm::vec2 values) override
// 	{
// 		const glm::vec2 delta = values - last_values;
// 		// std::cout << "moving mouse: " << delta.x << " " << delta.y << std::endl;
// 		pitch += 0.01 * delta.y;
// 		if (pitch > 3.1415/2.0)
// 			pitch = 3.1415/2.0;
// 		if (pitch < -3.1415/2.0)
// 			pitch = -3.1415/2.0;
// 	    program->set_uniform(u_camera_pitch, pitch);
// 		last_values = values;
// 	}
// 	virtual void on_mouse_button(int button, int action, int mods) override {}
// 	virtual void on_mouse_scroll(glm::vec2 values) override {}
// };

class Sky
{
public:
	struct Attributes {
		GLfloat position_x;
		GLfloat position_y;
	};
	const std::shared_ptr<const Program> program;
	const GLint u_camera_pitch;

	Sky(): program(Program::from_shaders(
	           Shader::from_file("../shaders/sky.vert", GL_VERTEX_SHADER),
	           Shader::from_file("../shaders/sky.frag", GL_FRAGMENT_SHADER))),
		   u_camera_pitch(program->get_uniform_location("u_camera_pitch")),
		   array_buffer(ArrayBuffer<Attributes>::create(buffer.size(), buffer.data())),
		   u_window_size(program->get_uniform_location("u_window_size")),
		   a_position(program->get_attribute_location("a_position"))
	{
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
	const GLint u_window_size;
	const GLint a_position;
};

class Object
{
public:
	const std::shared_ptr<const Program> program;
	const GLint u_mvp;
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

	Object(const std::string &path, const std::shared_ptr<const Program> &program):
		program(program),
		u_mvp(Object::program->get_uniform_location("u_mvp")),
		a_position(Object::program->get_attribute_location("a_position")),
		a_normal(Object::program->get_attribute_location("a_normal")),
		buffer(generate_attribute_sets(load(path).triangulate())),
		array_buffer(ArrayBuffer<Attributes>::create(buffer.size(), buffer.data()))
	{}

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

	const GLint a_position;
	const GLint a_normal;

    const std::vector<Attributes> buffer;
	const std::shared_ptr<const ArrayBuffer<Attributes>> array_buffer;
};

class Scene: public InputProvider::MouseListener
{
public:
	Scene(GLFWwindow *const window): window(window)
	{
		const std::shared_ptr<const Program> program = Program::from_shaders(
			Shader::from_file("../shaders/object.vert", GL_VERTEX_SHADER),
			Shader::from_file("../shaders/object.frag", GL_FRAGMENT_SHADER));
		objects.push_back(std::shared_ptr<Object>(new Object("../assets/suzanne.obj", program)));
		camera.translate({0.0f, 0.0f, 2.0f});
		camera.rotate({0.0f, 0.1f, 0.0f});
	}

	void render()
	{
		// FIXME shitting in main thread
		handle_input();

		glDisable(GL_DEPTH_TEST);
		sky.render();

		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::mat4 view_proj = camera.perspective() * camera.matrix();
		for (const auto &object: objects) {
			glm::mat4 mvp = view_proj * object->transformation.matrix();
			object->program->set_uniform(object->u_mvp, mvp);
			object->render();
		}
	}

	void handle_input()
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.translate({0.0f, 0.0f, -0.01f});
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.translate({0.0f, 0.0f, 0.01f});
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.translate({-0.01f, 0.0f, 0.0f});
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.translate({0.01f, 0.0f, 0.0f});
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			camera.translate({0.00f, 0.01f, 0.0f});
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			camera.translate({0.00f, -0.01f, 0.0f});
		}
	}

	virtual void on_mouse_move(glm::vec2 cursor_pos) override
	{
		const glm::vec2 delta = cursor_pos - last_cursor_pos;
		camera.rotate({-delta.y * 0.005f, -delta.x * 0.005f, 0.0f});
	    sky.program->set_uniform(sky.u_camera_pitch, camera.pitch());
		last_cursor_pos = cursor_pos;
	}

	virtual void on_mouse_button(int button, int action, int mods) override {}
	virtual void on_mouse_scroll(glm::vec2 values) override {}

protected:
	GLFWwindow *const window;
	Sky sky;
	Camera camera;
	glm::vec2 last_cursor_pos;
	std::vector<std::shared_ptr<Object>> objects;
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
    InputProvider::bind_callbacks(window);
    glfwSwapInterval(1);
	if (glewInit() != GLEW_OK)
        throw std::runtime_error("cannot initialize glew");

    Scene scene(window);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		scene.render();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    std::cout << "std::f*ck_it()" << std::endl;
    return 0;
}
