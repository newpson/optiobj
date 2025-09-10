#pragma once
#include <unordered_set>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class InputProvider
{
public:
	InputProvider() = delete;

	class KeyboardListener
	{
	public:
		KeyboardListener();
		~KeyboardListener();
		virtual void on_keyboard(int key, int action, int mods) = 0;
	};

	class MouseListener
	{
	public:
		MouseListener();
		~MouseListener();
		virtual void on_mouse_button(int button, int action, int mods) = 0;
		virtual void on_mouse_move(glm::vec2 values) = 0;
		virtual void on_mouse_scroll(glm::vec2 values) = 0;
	};

	static void bind_callbacks(GLFWwindow *const window);
	static void unbind_callbacks(GLFWwindow *const window);

protected:
	static std::unordered_set<KeyboardListener *> keyboard_listeners;
	static std::unordered_set<MouseListener *> mouse_listeners;
	static void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void glfw_mouse_move_callback(GLFWwindow *window, double xpos, double ypos);
	static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void glfw_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};
