#include "application.h"

namespace game {

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

  // Shaderプログラムの作成
  program_ = createProgram("shader.vert", "shader.frag");

  // Uniform変数の位置を取得
  model_loc_ = glGetUniformLocation(program_, "Model");
  model_it_loc_ = glGetUniformLocation(program_, "ModelIT");
  view_projection_loc_ = glGetUniformLocation(program_, "ViewProjection");
  world_camera_position_loc_ =
      glGetUniformLocation(program_, "worldCameraPosition");

  // Meshの読み込み
  auto mesh = Mesh::LoadObjMesh("sphere.obj");

  // Materialの作成
  auto material = std::make_shared<Material>(Texture("albedo.png", true),
                                             Texture("normal.png", false));

  // MeshEntityの作成
  mesh_entities_.emplace_back(mesh, material, glm::vec3(0.0f, 0.0f, 0.0f),
                              glm::vec3(0.0f), glm::vec3(1.0f));
  mesh_entities_.emplace_back(mesh, material, glm::vec3(2.0f, 0.0f, 0.0f),
                              glm::vec3(0.0f), glm::vec3(1.0f));
  mesh_entities_.emplace_back(mesh, material, glm::vec3(-2.0f, 0.0f, 0.0f),
                              glm::vec3(0.0f), glm::vec3(1.0f));

  // Cameraの作成
  camera_ = std::make_unique<Camera>(
      glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::radians(60.0f),
      static_cast<float>(width) / height, 0.1f, 100.0f);

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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program_);

  auto view_projection = camera_->GetViewProjectionMatrix();
  auto camera_position = camera_->GetPosition();

  glUniformMatrix4fv(view_projection_loc_, 1, GL_FALSE, &view_projection[0][0]);
  glUniform3fv(world_camera_position_loc_, 1, &camera_position[0]);

  for (auto&& mesh_entity : mesh_entities_) {
    auto model = mesh_entity.GetModelMatrix();
    auto model_it = glm::inverseTranspose(model);

    glUniformMatrix4fv(model_loc_, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(model_it_loc_, 1, GL_FALSE, &model_it[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  mesh_entity.material_->albedo_map_.GetTextureId());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,
                  mesh_entity.material_->normal_map_.GetTextureId());

    mesh_entity.mesh_->Draw();
  }
}

}  // namespace game