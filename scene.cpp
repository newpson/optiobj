#include "scene.hpp"

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderable.hpp"

using std::string;
using std::shared_ptr;

void Scene::add_renderable(const string &name, shared_ptr<Renderable> &&renderable)
{
    renderables.insert({name, renderable});
}

void Scene::render()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // TODO should i move these guys to setup?
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for (const auto pair: renderables) {
        pair.second->render();
    }
}