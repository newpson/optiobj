#pragma once
#include <map>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "renderable.hpp"

using std::map;
using std::string;
using std::shared_ptr;

class Scene
{
public:
    Scene() = default;
    void add_controllable(const string &name, shared_ptr<Renderable> &&controllable);
    void add_renderable(const string &name, shared_ptr<Renderable> &&renderable);

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void render();
protected:
    vec3 sun;
    map<string, shared_ptr<Renderable>> renderables;
    // map<string, shared_ptr<Controllable>> controllables;
};