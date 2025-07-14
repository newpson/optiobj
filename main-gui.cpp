#include <limits>
#include <stdexcept>
#include <limits>
#include <charconv>
#include <string>
#include <fstream>
#include <format>
#include <sstream>
#include <memory>
#include <iostream>

#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl2.h"

#include <glm/glm.hpp>
#include "mesh.hpp"
#include "parser.hpp"

using std::string;
using std::stringstream;
using std::ifstream;
using std::size_t;

struct InstantStatusStorage
{
    const size_t m_size;
    GLchar * const m_log;
    bool m_success = false;

    InstantStatusStorage(const size_t size = 512)
        : m_size(size * sizeof(GLchar)), m_log(new GLchar [size])
    {}

    ~InstantStatusStorage()
    {
        delete [] m_log;
    }

    operator GLchar * const * () { return &m_log; }
    operator char * () { return m_log; }
    const char * to_string() { return m_log; }
    operator bool () { return m_success; }
    operator int * () { return reinterpret_cast<int *>(&m_success); }
    size_t size() { return m_size; }
};

class GLShader
{
private:
    GLuint m_type = -1;
    GLuint m_id = -1;

public:
    GLShader(const GLShader &shader) = delete;

    GLShader(GLFWwindow * const context, const string &path, const int type)
        : m_type(type)
    {
        ifstream input(path, std::ios::in);
        if (!input.is_open())
            throw std::runtime_error(std::format("cannot read \"{}\"", path));
        stringstream source_stream;
        source_stream << input.rdbuf();
        const string source = source_stream.str();
        input.close();

        glfwMakeContextCurrent(context);
        m_id = glCreateShader(type);
        if (m_id <= 0)
            throw std::runtime_error("cannot allocate shader handle");

        const GLchar * const source_cstr = source.c_str();
      	glShaderSource(m_id, 1, &source_cstr, NULL);

        InstantStatusStorage status;
        glCompileShader(m_id);
        glGetShaderiv(m_id, GL_COMPILE_STATUS, status);
        if (!status) {
            glGetShaderInfoLog(m_id, sizeof(status.size()), NULL, status);
            throw std::runtime_error(std::format(
                "shader compilation error ({}):\n{}", path, status.to_string()));
        }
    }

    ~GLShader()
    {
        // FIXME context
        std::cerr << "[DEBUG] deleting shader " << m_id << std::endl;
      	glDeleteShader(m_id);
    }

    GLuint type() const { return m_type; }
    GLuint id() const { return m_id; }
    operator GLuint () const { return m_id; }
};

class GLProgram
{
private:
    GLuint m_id;

public:
    GLProgram(): m_id(0) {}

    GLProgram(const GLProgram &program) = delete;

    // allow only move constructor (aka "do not call destructor on copy")
    GLProgram(const GLProgram &&program)
        : m_id(program.m_id)
    {}

    GLProgram(GLFWwindow *context, const GLShader &shader_vertex, const GLShader &shader_fragment)
    {
        glfwMakeContextCurrent(context);
        m_id = glCreateProgram();
        if (m_id <= 0)
            throw std::runtime_error("cannot allocate program handle");
        glAttachShader(m_id, shader_vertex);
        glAttachShader(m_id, shader_fragment);
        glLinkProgram(m_id);
        InstantStatusStorage status;
        glGetProgramiv(m_id, GL_LINK_STATUS, status);
        if (!status)
        {
            glGetProgramInfoLog(m_id, status.size(), NULL, status);
            throw std::runtime_error(std::format("program linkage error:\n{}", status.to_string()));
        }
    }

    ~GLProgram()
    {
        // FIXME context
        std::cerr << "[DEBUG] deleting program " << m_id << std::endl;
        glDeleteProgram(m_id);
    }

    GLuint id() { return m_id; }
    operator GLuint () { return m_id; }
};

class Renderable
{
public:
    virtual void render(GLFWwindow *context) = 0;
};

class GLObject: public Renderable
{
    string name;
    const std::shared_ptr<GLProgram> program;
    vector<GLuint> vbos; // TODO create separate VBO class
    // Transformation ...
public:
    GLObject(GLFWwindow *context, const Mesh &mesh, const std::shared_ptr<GLProgram> &program)
        : program(program)
    {
        // TODO maybe check if mesh is triangulated and throw exception if not
        // TODO fill VBOs with data
    }

    void render(GLFWwindow *context) override
    {
        glfwMakeContextCurrent(context);
        glClearColor(1.0, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(*program.get());
        // glUniformMatrix4fv(view, sizeof(float), 16, false, data)
        // glUniformMatrix4fv(direction, sizeof(float), 16, false, data)
        // m_program.setUniformValue(m_viewLoc, m_camera.view());
        // m_program.setUniformValue(m_dirLoc, m_camera.direction());

        // for (vbo : vbos)
        //     glDrawArrays(GL_TRIANGLES, 0, vbo.size_bytes());
    }
};

void setup_gl(GLFWwindow * const context)
{
    glfwMakeContextCurrent(context);
	if (glewInit() != GLEW_OK)
        throw std::runtime_error("cannot initialize glew");
	// set vsync
    glfwSwapInterval(1);
}

void setup_imgui(GLFWwindow * const context)
{
    glfwMakeContextCurrent(context);
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = nullptr; // disable settings.ini
	ImGui_ImplGlfw_InitForOpenGL(context, true);
	ImGui_ImplOpenGL2_Init();
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
    ImGui_ImplOpenGL2_NewFrame();
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
                for (int i = 0; i < 10; ++i) {
                    std::string string_id = "      ";
                    std::to_chars(string_id.begin().base(), string_id.end().base(), i);
                    const std::string node_id = "node " + string_id;
                    if (ImGui::TreeNodeEx(node_id.c_str(), ImGuiTreeNodeFlags_Leaf))
                        ImGui::TreePop();
                }
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

void push_gl(GLFWwindow * const context)
{
    glfwMakeContextCurrent(context);
    glClearColor(0.8, 0.8, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void destroy_ui(GLFWwindow * const context)
{
    glfwMakeContextCurrent(context);
    ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void destroy_gl(GLFWwindow * const context)
{
    glfwMakeContextCurrent(context);
    glfwDestroyWindow(context);
	glfwTerminate();
}

int main()
{
   	if (!glfwInit())
		throw std::runtime_error("cannot initialize glfw");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow *context = glfwCreateWindow(800, 600, "optiobj", nullptr, nullptr);
	if (context == nullptr)
		throw std::runtime_error("cannot create context");

    setup_gl(context);
    Mesh mesh = load("cube-grouped.obj");
    GLObject object(context, mesh.triangulate(),
            std::shared_ptr<GLProgram>(new GLProgram(context,
                GLShader(context, "geometry.vert", GL_VERTEX_SHADER),
              GLShader(context, "color.frag", GL_FRAGMENT_SHADER))));

    setup_imgui(context);
	while (!glfwWindowShouldClose(context))
	{
        glfwPollEvents();

        // push_gl(context);
        push_ui(context);
        // object.render(context);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(context);
    }


    return 0;
}