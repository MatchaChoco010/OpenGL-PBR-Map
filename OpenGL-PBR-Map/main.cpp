#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main() {
  // GLFW エラーのコールバック
  glfwSetErrorCallback(
      [](auto id, auto description) { std::cerr << description << std::endl; });

  const GLuint width = 960;
  const GLuint height = 540;

  // GLFWの初期化
  if (!glfwInit()) {
    return false;
  }

  // OpenGL Version 4.6 Core Profileを選択する
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // リサイズ不可
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // ウィンドウの作成
  GLFWwindow* window =
      glfwCreateWindow(width, height, "Game", nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Can't create GLFW window." << std::endl;
    return false;
  }
  glfwMakeContextCurrent(window);

  // GLEWの初期化
  if (glewInit() != GLEW_OK) {
    std::cerr << "Can't initialize GLEW." << std::endl;
    return false;
  }

  // VSyncを待つ
  glfwSwapInterval(1);

  // OpenGL エラーのコールバック
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(
      [](auto source, auto type, auto id, auto severity, auto length,
         const auto* message, const void* userParam) {
        auto t = type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "";
        std::cerr << "GL CALLBACK: " << t << " type = " << type
                  << ", severity = " << severity << ", message = " << message
                  << std::endl;
      },
      0);

  // メインループ
  while (glfwWindowShouldClose(window) == GL_FALSE) {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}