#include "inputprovider.hpp"
#include <GLFW/glfw3.h>

std::unordered_set<InputProvider::KeyboardListener *> InputProvider::keyboard_listeners;
std::unordered_set<InputProvider::MouseListener *> InputProvider::mouse_listeners;

InputProvider::KeyboardListener::KeyboardListener()
{
	InputProvider::keyboard_listeners.insert(this);
}

InputProvider::KeyboardListener::~KeyboardListener()
{
	InputProvider::keyboard_listeners.erase(this);
}

InputProvider::MouseListener::MouseListener()
{
	InputProvider::mouse_listeners.insert(this);
}

InputProvider::MouseListener::~MouseListener()
{
	InputProvider::mouse_listeners.erase(this);
}

void InputProvider::bind_callbacks(GLFWwindow *const window)
{
    glfwSetKeyCallback(window, InputProvider::glfw_keyboard_callback);
    glfwSetCursorPosCallback(window, InputProvider::glfw_mouse_move_callback);
    glfwSetMouseButtonCallback(window, InputProvider::glfw_mouse_button_callback);
    glfwSetScrollCallback(window, InputProvider::glfw_mouse_scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputProvider::unbind_callbacks(GLFWwindow *const window)
{
    glfwSetKeyCallback(window, nullptr);
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwSetScrollCallback(window, nullptr);
}

void InputProvider::glfw_keyboard_callback(GLFWwindow *window,
                                           int key, int scancode, int action, int mods)
{
	for (const auto &listener: InputProvider::keyboard_listeners)
	    listener->on_keyboard(key, action, mods);
}

void InputProvider::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	for (const auto &listener: InputProvider::mouse_listeners)
	    listener->on_mouse_button(button, action, mods);
}

void InputProvider::glfw_mouse_move_callback(GLFWwindow *window, double xpos, double ypos)
{
	for (const auto &listener: InputProvider::mouse_listeners)
	    listener->on_mouse_move({xpos, ypos});
}

void InputProvider::glfw_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	for (const auto &listener: InputProvider::mouse_listeners)
	    listener->on_mouse_move({xoffset, yoffset});
}
