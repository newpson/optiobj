#include <cstddef>
#include <iostream>
#include <glm/fwd.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "program-configurator/default.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "renderable.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "status.hpp"
#include "transformation.hpp"
#include "utility.hpp"
#include "vbo.hpp"

#include <unistd.h>

using std::string;
using std::stringstream;
using std::ifstream;
using std::size_t;
using glm::mat4;
using glm::vec3;
using glm::vec4;



GLFWwindow *setup_gl()
{
    if (!glfwInit())
		throw std::runtime_error("cannot initialize glfw");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow *context = glfwCreateWindow(800, 600, "optiobj", nullptr, nullptr);
	if (context == nullptr)
		throw std::runtime_error("cannot create context");
    glfwMakeContextCurrent(context);
	if (glewInit() != GLEW_OK)
        throw std::runtime_error("cannot initialize glew");
    glfwSwapInterval(1);
    return context;
}

void setup_imgui(GLFWwindow * const context)
{
    glfwMakeContextCurrent(context);
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = nullptr; // disable settings.ini
	ImGui_ImplGlfw_InitForOpenGL(context, true);
	ImGui_ImplOpenGL3_Init("#version 110");
}

void push_ui(GLFWwindow * const context)
{
    // inputs
    static float loading_progress = 0.0f;
    // outputs
    static int camera_type = 0;
    static glm::vec3 location(0.0f, 0.0f, 0.0f);
    static glm::vec3 rotation(0.0f, 0.0f, 0.0f);
    static glm::vec3 scale(1.0f, 1.0f, 1.0f);

    glfwMakeContextCurrent(context);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::SetNextWindowSize({200, 600});
        ImGui::SetNextWindowPos({0, 0});
        // ImGui::SetNextWindowCollapsed(true); // can't maximize back for some reason
        const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove
                                            | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Control panel", nullptr, window_flags);
            ImGui::SeparatorText("Settings");
            ImGui::Text("Camera");
                ImGui::RadioButton("FPV", &camera_type, 0);
                ImGui::SameLine();
                ImGui::RadioButton("TPV", &camera_type, 1);

            ImGui::SeparatorText("Objects");
            // ImGui::BeginTable("objects", 3);
            // ImGui::TableNextRow();
            // ImGui::TableNextColumn();
            // ImGui::Text("hello");
            // ImGui::TableNextColumn();
            // ImGui::Text("world");
            // ImGui::TableNextColumn();
            // ImGui::Text("this is a test");
            // ImGui::EndTable();

            if (ImGui::TreeNodeEx("root", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected)) {
                ImGui::TreePop();
            }
            ImGui::Button("Load");
            ImGui::SameLine();
            ImGui::Button("Delete");


            ImGui::SeparatorText("Object properties");
            ImGui::Text("Location");
                ImGui::DragFloat("x##loc", &location.x);
                ImGui::DragFloat("y##loc", &location.y);
                ImGui::DragFloat("z##loc", &location.z);
            ImGui::Text("Rotation");
                ImGui::DragFloat("x##rot", &rotation.x, 0.05f);
                ImGui::DragFloat("y##rot", &rotation.y, 0.05f);
                ImGui::DragFloat("z##rot", &rotation.z, 0.05f);
            ImGui::Text("Scale");
                ImGui::DragFloat("x##sca", &scale.x, 0.05f, 0.0, std::numeric_limits<float>::infinity());
                ImGui::DragFloat("y##sca", &scale.y, 0.05f, 0.0f, std::numeric_limits<float>::infinity());
                ImGui::DragFloat("z##sca", &scale.z, 0.05f, 0.0f, std::numeric_limits<float>::infinity());
        ImGui::End();
    }
    // Loading dialog
    // {
    //     ImGui::SetNextWindowSize({0, 0});
    //     const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove
    //                                         | ImGuiWindowFlags_NoResize
    //                                         | ImGuiWindowFlags_NoCollapse;
    //     ImGui::Begin("Loading", nullptr, window_flags);
    //     ImGui::SetNextItemWidth(200.0f);
    //     ImGui::ProgressBar(-0.5f * (float)ImGui::GetTime(), ImVec2(0.0f, 0.0f), "/home/newpson/object.obj");
    //     ImGui::End();
    // }
    ImGui::Render();
}

void destroy_ui(GLFWwindow * const context)
{
    glfwMakeContextCurrent(context);
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void destroy_gl(GLFWwindow * const context)
{
    glfwMakeContextCurrent(context);
    glfwDestroyWindow(context);
	glfwTerminate();
}

struct Attributes
{
    GLfloat position_x;
    GLfloat position_y;

    Attributes(const vec2 &position);
    static constexpr size_t num_floats();
};

int main()
{
    GLFWwindow *context = setup_gl();
    glfwMakeContextCurrent(context);
    // Mesh mesh = load("../suzanne.obj");

    setup_imgui(context);

    // auto program_object = Program::from_shaders(
    //     Shader::from_file("../object.vert", GL_VERTEX_SHADER),
    //     Shader::from_file("../object.frag", GL_FRAGMENT_SHADER));
    // ProgramConfigurators::Sky configurator(program_object);
    // Scene scene;
    // scene.camera.translate({0.0f, 0.0f, 0.0f});
    // configurator.set_mvp(scene.camera.perspective() * scene.camera.matrix());
    // scene.add_renderable(
    //     "hello",
    //     shared_ptr<Renderable>(new Object(program_object, configurator.generate_vbos(mesh.triangulate()))));
    // Program sky_program(shader_v2, shader_f2);
    // Object object(mesh1, generic);
    // Object object_sky(mesh2, sky_program);
    // glfwSetKeyCallback(window, object.key_callback)
    // scene.add("sky", object);
    // Scene scene;
    // scene.add("my_object", object);

    const auto program = Program::from_shaders(
        Shader::from_file("../sky.vert", GL_VERTEX_SHADER),
        Shader::from_file("../sky.frag", GL_FRAGMENT_SHADER));

    const auto vbo = VertexBufferObject::create("sky-plane", {
        1.0f, 1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, -1.0f,
      }, 4);

    GLint u_camera_pitch = program->get_uniform_location("camera_pitch");
    GLint u_window_size = program->get_uniform_location("window_size");
    program->set_uniform(u_window_size, vec2(800.0f, 600.0f));
    program->set_uniform(u_camera_pitch, 0.5f);

    GLint a_position = program->get_attribute_location("position");
    glBindBuffer(GL_ARRAY_BUFFER, vbo->id());
    glVertexAttribPointer(a_position, vec2::length(), GL_FLOAT,
        GL_FALSE, sizeof(Attributes),
        reinterpret_cast<void *>(offsetof(Attributes, position_x)));

	while (!glfwWindowShouldClose(context))
	{
		glfwPollEvents();

		glClearColor(1.0, 1.0, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

        push_ui(context);

        glUseProgram(program->id());
        glBindBuffer(GL_ARRAY_BUFFER, vbo->id());
        glEnableVertexAttribArray(a_position);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vbo->num_indices());
        glDisableVertexAttribArray(a_position);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(context);
    }

    destroy_ui(context);

    return 0;
}