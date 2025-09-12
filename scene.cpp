#include "scene.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "camera.hpp"
#include "object.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "sky.hpp"
#include "transformation.hpp"

Scene::Scene(GLFWwindow *const window,
             const std::shared_ptr<const Program> program):
    window(window), program(program)
{
	objects.push_back(std::shared_ptr<Object>(new Object("../assets/suzanne.obj", program)));
	objects.push_back(std::shared_ptr<Object>(new Object("../assets/cube.obj", program)));
	objects.push_back(std::shared_ptr<Object>(new Object("../assets/skull.obj", program)));
	camera.translate({0.0f, 0.0f, 2.0f});
	camera.rotate({0.0f, 0.1f, 0.0f});
}

void Scene::render()
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

void Scene::handle_input()
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

void Scene::on_mouse_move(glm::vec2 cursor_pos)
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

void Scene::on_keyboard(int key, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_RIGHT:
			next_object();
			break;
		case GLFW_KEY_LEFT:
			previous_object();
			break;
		case GLFW_KEY_L:
			load_object();
			break;
		case GLFW_KEY_DELETE:
			unload_object();
			break;
		}
	}
}

void Scene::on_mouse_button(int button, int action, int mods)
{}

void Scene::on_mouse_scroll(glm::vec2 values)
{}

void Scene::next_object()
{
	selected_object = (selected_object + 1) % objects.size();
}

void Scene::previous_object()
{
	selected_object = (selected_object == 0 ? objects.size() - 1 : selected_object - 1);
}

void Scene::load_object()
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

void Scene::unload_object()
{
	if (objects.size() > 0) {
		objects.erase(objects.begin() + selected_object);
	    std::cout << "Object #" << selected_object << " unloaded." << std::endl;
	    previous_object();
    } else {
	    std::cout << "No more objects left. " << std::endl;
    }
}
