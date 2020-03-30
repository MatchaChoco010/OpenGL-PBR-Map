#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>

GLuint createProgram(std::string vertexShaderFile,
                     std::string fragmentShaderFile) {
  // 頂点シェーダの読み込み
  std::ifstream vertexIfs(vertexShaderFile, std::ios::binary);
  if (vertexIfs.fail()) {
    std::cerr << "Error: Can't open source file: " << vertexShaderFile
              << std::endl;
    return 0;
  }
  auto vertexShaderSource =
      std::string(std::istreambuf_iterator<char>(vertexIfs),
                  std::istreambuf_iterator<char>());
  if (vertexIfs.fail()) {
    std::cerr << "Error: Can't read source file: " << vertexShaderFile
              << std::endl;
    return 0;
  }
  const GLchar* vertexShaderSourcePointer = vertexShaderSource.c_str();

  // フラグメントシェーダの読み込み
  std::ifstream fragmentIfs(fragmentShaderFile, std::ios::binary);
  if (fragmentIfs.fail()) {
    std::cerr << "Error: Can't open source file: " << fragmentShaderFile
              << std::endl;
    return 0;
  }
  auto fragmentShaderSource =
      std::string(std::istreambuf_iterator<char>(fragmentIfs),
                  std::istreambuf_iterator<char>());
  if (fragmentIfs.fail()) {
    std::cerr << "Error: Can't read source file: " << fragmentShaderFile
              << std::endl;
    return 0;
  }
  const GLchar* fragmentShaderSourcePointer = fragmentShaderSource.c_str();

  // プログラムオブジェクトを作成
  const GLuint program = glCreateProgram();

  GLint status = GL_FALSE;
  GLsizei infoLogLength;

  // 頂点シェーダのコンパイル
  const GLuint vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderObj, 1, &vertexShaderSourcePointer, nullptr);
  glCompileShader(vertexShaderObj);
  glAttachShader(program, vertexShaderObj);

  // 頂点シェーダのチェック
  glGetShaderiv(vertexShaderObj, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
    std::cerr << "Compile Error in Vertex Shader." << std::endl;
  glGetShaderiv(vertexShaderObj, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 1) {
    std::vector<GLchar> vertexShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(vertexShaderObj, infoLogLength, nullptr,
                       vertexShaderErrorMessage.data());
    std::cerr << vertexShaderErrorMessage.data() << std::endl;
  }

  glDeleteShader(vertexShaderObj);

  // フラグメントシェーダのコンパイル
  const GLuint fragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderObj, 1, &fragmentShaderSourcePointer, nullptr);
  glCompileShader(fragmentShaderObj);
  glAttachShader(program, fragmentShaderObj);

  // フラグメントシェーダのチェック
  glGetShaderiv(fragmentShaderObj, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
    std::cerr << "Compile Error in Fragment Shader." << std::endl;
  glGetShaderiv(fragmentShaderObj, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 1) {
    std::vector<GLchar> fragmentShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(fragmentShaderObj, infoLogLength, nullptr,
                       fragmentShaderErrorMessage.data());
    std::cerr << fragmentShaderErrorMessage.data() << std::endl;
  }

  glDeleteShader(fragmentShaderObj);

  // プログラムのリンク
  glLinkProgram(program);

  // リンクのチェック
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 1) {
    std::vector<GLchar> programLinkErrorMessage(infoLogLength);
    glGetProgramInfoLog(program, infoLogLength, nullptr,
                        programLinkErrorMessage.data());
    std::cerr << programLinkErrorMessage.data() << std::endl;
  }

  return program;
}

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

  // Shaderプログラムの作成
  const GLuint program = createProgram("shader.vert", "shader.frag");

  // 表示する三角形の作成
  const std::vector<glm::vec3> vertices = {
      glm::vec3(0.0f, 0.5f, 0.0f),
      glm::vec3(-0.5f, -0.5f, 0.0f),
      glm::vec3(0.5f, -0.5f, 0.0f),
  };
  const std::vector<glm::vec3> colors = {
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),
  };

  // VAOの作成とバインド
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // VBOを作成
  GLuint verticesVbo;
  glGenBuffers(1, &verticesVbo);
  glBindBuffer(GL_ARRAY_BUFFER, verticesVbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
               &vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  GLuint colorsVbo;
  glGenBuffers(1, &colorsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, colorsVbo);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0],
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  // メインループ
  while (glfwWindowShouldClose(window) == GL_FALSE) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}