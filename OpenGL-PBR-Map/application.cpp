#include "application.h"

namespace game {

bool Application::Run() {
  if (!Init()) {
    std::cerr << "Initialization error..." << std::endl;
    return false;
  }

  glfwSetTime(0.0);
  double delta_time = 0.0;
  double prev_time = 0.0;

  while (glfwWindowShouldClose(window_) == GL_FALSE) {
    const double time = glfwGetTime();
    delta_time = time - prev_time;
    prev_time = time;

    Update(delta_time);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }

  glfwTerminate();

  return true;
}

bool Application::Init() {
  const GLuint width = 960;
  const GLuint height = 540;

  if (!InitWindow(width, height)) {
    std::cerr << "Error: InitWindow" << std::endl;
    return false;
  }

  // Sceneの読み込み
  // scene_ = Scene::LoadScene("SceneFile.scenefile", width, height);
  // scene_->camera_ = std::make_unique<Camera>(
  //    glm::vec3(-1.37508f, 7.96885f, 21.19848),
  //    glm::vec3(glm::radians(73.2f - 90.0f), glm::radians(-4.61f),
  //              -glm::radians(-0.000004f)),
  //    glm::radians(30.0f), static_cast<GLfloat>(width) / height, 0.1f,
  //    150.0f);
  scene_ = Scene::LoadScene("test-ibl.scenefile", width, height);
  scene_->camera_ = std::make_unique<Camera>(
      glm::vec3(-1.37508f, 7.96885f, 21.19848),
      glm::vec3(glm::radians(73.2f - 90.0f), glm::radians(-4.61f),
                -glm::radians(-0.000004f)),
      glm::radians(30.0f), static_cast<GLfloat>(width) / height, 0.1f, 150.0f);

  // SceneRendererの作成
  scene_renderer_ = std::make_unique<SceneRenderer>(width, height);
  scene_renderer_->SetEvComp(0.0f);

  return true;
}

bool Application::InitWindow(const GLuint width, const GLuint height) {
  glfwSetErrorCallback(
      [](auto id, auto description) { std::cerr << description << std::endl; });

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
  window_ = glfwCreateWindow(width, height, "Game", nullptr, nullptr);
  if (window_ == nullptr) {
    std::cerr << "Can't create GLFW window." << std::endl;
    return false;
  }
  glfwMakeContextCurrent(window_);

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

  glEnable(GL_DEPTH_TEST);

  return true;
}

void Application::Update(const double delta_time) {
  scene_renderer_->Render(*scene_, delta_time);
}

}  // namespace game