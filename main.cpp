#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
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

class Sky
{
public:
	struct Attributes {
		GLfloat position_x;
		GLfloat position_y;
	};
	const std::shared_ptr<const Program> program;
	const GLint u_camera_pitch;

	// FIXME hardcoded shader paths
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

	Object(const std::string &path, const std::shared_ptr<const Program> &program):
		program(program),
		u_mvp(Object::program->get_uniform_location("u_mvp")),
		u_local_rotation(Object::program->get_uniform_location("u_local_rotation")),
		u_is_selected(Object::program->get_uniform_location("u_is_selected")),
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

class Scene: public InputProvider::MouseListener,
			 public InputProvider::KeyboardListener
{
public:
	Scene(GLFWwindow *const window,
	      const std::shared_ptr<const Program> program):
		window(window), program(program)
	{
		objects.push_back(std::shared_ptr<Object>(new Object("../assets/suzanne.obj", program)));
		objects.push_back(std::shared_ptr<Object>(new Object("../assets/cube.obj", program)));
		objects.push_back(std::shared_ptr<Object>(new Object("../assets/skull.obj", program)));
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
		for (std::size_t i = 0; i < objects.size(); ++i) {
			glm::mat4 mvp = view_proj * objects[i]->transformation.matrix();
			if (i == selected_object)
				objects[i]->program->set_uniform(objects[i]->u_is_selected, 1.0f);
			else
				objects[i]->program->set_uniform(objects[i]->u_is_selected, 0.0f);
			objects[i]->program->set_uniform(objects[i]->u_mvp, mvp);
			glm::mat4 local_rotation = glm::identity<glm::mat4>();
			local_rotation = Transformation::rotate(local_rotation,
			                                        objects[i]->transformation.rotation);
			objects[i]->program->set_uniform(objects[i]->u_local_rotation,
			                                 local_rotation);
			objects[i]->render();
		}
	}

	void handle_input()
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.translate({0.0f, 0.0f, -0.03f});
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.translate({0.0f, 0.0f, 0.03f});
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.translate({-0.03f, 0.0f, 0.0f});
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.translate({0.03f, 0.0f, 0.0f});
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			camera.translate({0.00f, 0.03f, 0.0f});
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			camera.translate({0.00f, -0.03f, 0.0f});
		}
	}

	virtual void on_mouse_move(glm::vec2 cursor_pos) override
	{
		const glm::vec2 delta = cursor_pos - last_cursor_pos;
		const float sens = 0.005f;
		last_cursor_pos = cursor_pos;
		if (glfwGetKey(window, GLFW_KEY_G)) {
			objects[selected_object]->transformation.position
				+= glm::vec3(delta.x*sens, 0.0f, delta.y*sens);
			return;
		} else if (glfwGetKey(window, GLFW_KEY_R)) {
			objects[selected_object]->transformation.rotation
				+= glm::vec3(delta.y*sens, delta.x*sens, 0.0f);
			return;
		} else if (glfwGetKey(window, GLFW_KEY_T)) {
			const float scale = delta.x*sens;
			objects[selected_object]->transformation.scale
				+= glm::vec3(scale, scale, scale);
			return;
		}
		camera.rotate({-delta.y*sens, -delta.x*sens, 0.0f});
	    sky.program->set_uniform(sky.u_camera_pitch, camera.pitch());
	}

	virtual void on_keyboard(int key, int action, int mods) override
	{
		if (action == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_RIGHT:
				next_object();
				break;
			case GLFW_KEY_LEFT:
				previous_object();
				break;
			case GLFW_KEY_DOWN:
				load_object();
				break;
			case GLFW_KEY_UP:
				unload_object();
				break;
			}
		}
	}

	virtual void on_mouse_button(int button, int action, int mods) override {}
	virtual void on_mouse_scroll(glm::vec2 values) override {}

	void next_object()
	{
		selected_object = (selected_object + 1) % objects.size();
	}

	void previous_object()
	{
		selected_object = (selected_object == 0 ? objects.size() - 1 : selected_object - 1);
	}

	void load_object()
	{
	    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	    std::cout << "Interrupt! Object loading mode. Enter .obj file path:\n> ";
		std::string path;
	    std::getline(std::cin, path);
	    if (!path.empty()) {
	    	try {
		    	objects.push_back(std::shared_ptr<Object>(new Object(path, program)));
				std::cout << "Successfully loaded." << std::endl;;
				selected_object = objects.size() - 1;
		    } catch (const ParsingError &err) {
		    	std::cout << "Unable to load \"" << path << "\":\n"
		    			  << err.message(err.type) << " at " << err.line << ":" << err.column
		    			  << std::endl;
		    }
		} else {
			std::cout << "Path is empty." << std::endl;;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void unload_object()
	{
		if (objects.size() > 0) {
			objects.erase(objects.begin() + selected_object);
		    std::cout << "Object #" << selected_object << " unloaded." << std::endl;
		    previous_object();
	    } else {
		    std::cout << "No more objects left. " << std::endl;
	    }
	}

protected:
	GLFWwindow *const window;
	const std::shared_ptr<const Program> program;
	Sky sky;
	Camera camera;
	glm::vec2 last_cursor_pos;
	std::vector<std::shared_ptr<Object>> objects;
	std::size_t selected_object = 0;
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

	const std::shared_ptr<const Program> program = Program::from_shaders(
		Shader::from_file("../shaders/object.vert", GL_VERTEX_SHADER),
		Shader::from_file("../shaders/object.frag", GL_FRAGMENT_SHADER));
    Scene scene(window, program);
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
