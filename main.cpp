#include <stdexcept>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "inputprovider.hpp"
#include "program.hpp"
#include "scene.hpp"
#include "shader.hpp"

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
    return 0;
}
