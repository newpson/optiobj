#pragma once
#include <cstddef>
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "inputprovider.hpp"
#include "object.hpp"
#include "program.hpp"
#include "sky.hpp"

class Scene: public InputProvider::MouseListener,
			 public InputProvider::KeyboardListener
{
public:
	Scene(GLFWwindow *const window,
	      const std::shared_ptr<const Program> program);

	void render();

	void handle_input();
	virtual void on_mouse_move(glm::vec2 cursor_pos) override;
	virtual void on_keyboard(int key, int action, int mods) override;
	virtual void on_mouse_button(int button, int action, int mods) override;
	virtual void on_mouse_scroll(glm::vec2 values) override;

	void next_object();
	void previous_object();
	void load_object();
	void unload_object();

protected:
	GLFWwindow *const window;
	const std::shared_ptr<const Program> program;
	Sky sky;
	Camera camera;
	glm::vec2 last_cursor_pos;
	std::vector<std::shared_ptr<Object>> objects;
	std::size_t selected_object = 0;
};
