#ifndef SCREEN_H
#define SCREEN_H

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include "imgui.h"

class Screen {
 public:
  static int init(int size_x, int size_y);
  static int clean_up();
  static void frame_start();
  static void frame_end();
  static bool should_close();

  static ImGuiIO* io;  
  static GLFWwindow* window;  
  static std::pair<size_t, size_t> get_window_size(); 

  private:
  static Screen& instance();

  static int size_x; 
  static int size_y; 

  static void framebuffer_size_callback(GLFWwindow* window, int width, int heigth);
};

#endif  // SCREEN_H