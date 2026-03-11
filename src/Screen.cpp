#include <fmt/core.h>
#include <iostream>
#include <string>

#include "Screen.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

/// Default ImGui configuration
static const char *default_imgui_ini = R"ini(
[Window][WindowOverViewport_11111111]
Pos=0,0
Size=1080,720
Collapsed=0

[Window][Debug##Default]
Pos=60,10
Size=400,400
Collapsed=0

[Window][Test]
Size=1080,720
Collapsed=0
DockId=0x00000001,0

[Window][Files]
Pos=0,0
Size=362,720
Collapsed=0
DockId=0x00000001,0

[Window][Data]
Pos=381,0
Size=699,720
Collapsed=0
DockId=0x00000002,0

[Window][Variable Plot]
Pos=364,253
Size=716,467
Collapsed=0
DockId=0x00000006,0

[Window][Variable]
Pos=364,0
Size=716,251
Collapsed=0
DockId=0x00000005,0

[Docking][Data]
DockSpace     ID=0x11111111 Window=0x1BBC0F80 Pos=0,0 Size=1080,720 Split=X Selected=0x0469CA7A
  DockNode    ID=0x00000003 Parent=0x11111111 SizeRef=362,720 Split=X
    DockNode  ID=0x00000001 Parent=0x00000003 SizeRef=379,987 CentralNode=1 Selected=0x0469CA7A
    DockNode  ID=0x00000002 Parent=0x00000003 SizeRef=699,987 Selected=0xA5D05E63
  DockNode    ID=0x00000004 Parent=0x11111111 SizeRef=716,720 Split=Y Selected=0x50426C07
    DockNode  ID=0x00000005 Parent=0x00000004 SizeRef=670,251 Selected=0x50426C07
    DockNode  ID=0x00000006 Parent=0x00000004 SizeRef=670,467 Selected=0xF95C384F


)ini";

ImGuiIO *Screen::io = nullptr;
GLFWwindow *Screen::window = nullptr;
int Screen::size_x = -1;
int Screen::size_y = -1;

void error_callback(int error, const char *description)
{
    std::string msg = fmt::format("GLFW error {}, {}\n", error, description);
    fmt::print(msg);
}

// Viewport resize callback
void Screen::framebuffer_size_callback(GLFWwindow *window, int width, int heigth)
{
    glViewport(0, 0, width, heigth);
    Screen::size_x = width;
    Screen::size_y = heigth;
}

Screen &instance()
{
    static Screen screen;
    return screen;
}

int Screen::init(int size_x, int size_y)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        fmt::print("GLFW init failed\n");
        return 1;
    }

    // Set viewport size and resize
    if (size_x < 0 || size_y < 0)
    {
        // Get primary monitor resolution
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        size_x = mode->width;
        size_y = mode->height;
    }

    // Set the static screen size variables
    Screen::size_x = size_x;
    Screen::size_y = size_y;

    Screen::window = glfwCreateWindow(size_x, size_y, "nc-inspect", NULL, NULL);

    if (!Screen::window)
    {
        fmt::print("GLFW window creation failed.\n");
        return 1;
    }

    glViewport(0, 0, size_x, size_y);
    glfwSetFramebufferSizeCallback(Screen::window, framebuffer_size_callback);

    glfwMakeContextCurrent(Screen::window);

    glfwSetInputMode(Screen::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    Screen::io = &ImGui::GetIO();
    Screen::io->IniFilename = nullptr;
    ImGui::LoadIniSettingsFromMemory(default_imgui_ini);
    Screen::io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Screen::window, true); // Second param install_callback=true will install
                                                        // GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    return 0;
}

int Screen::clean_up()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void Screen::frame_start()
{
    if (glfwGetKey(Screen::window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Screen::window, true);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    glClearColor(0.12f, 0.13f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr,
                                 ImGuiDockNodeFlags_PassthruCentralNode);
}

void Screen::frame_end()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(Screen::window);
    glfwPollEvents();
}

bool Screen::should_close()
{
    return glfwWindowShouldClose(Screen::window);
}

std::pair<size_t, size_t> Screen::get_window_size()
{
    return {size_x, size_y};
}