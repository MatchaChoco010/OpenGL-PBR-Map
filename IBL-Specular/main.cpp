#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

GLFWwindow* window;

const GLuint LoadExr(const std::string& path) {
  float* data;
  GLuint texture_id = 0;
  int width;
  int height;
  const char* err = nullptr;
  int ret = LoadEXR(&data, &width, &height, path.c_str(), &err);
  if (ret != TINYEXR_SUCCESS) {
    std::cerr << "Can't load image: " << path << std::endl;
    if (err) {
      std::cerr << "ERR : " << err << std::endl;
      FreeEXRErrorMessage(err);
    }
  } else {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                 GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
  }
  return texture_id;
}

bool SaveExr(std::unique_ptr<float[]> data, const std::string& path,
             const int width, const int height) {
  EXRHeader header;
  InitEXRHeader(&header);

  EXRImage image;
  InitEXRImage(&image);

  image.num_channels = 3;

  std::vector<float> images[3];
  images[0].resize(width * height);
  images[1].resize(width * height);
  images[2].resize(width * height);

  // Split RGBARGBARGBA... into R, G and B layer
  for (int i = 0; i < width * height; i++) {
    images[0][i] = data[4 * i + 0];
    images[1][i] = data[4 * i + 1];
    images[2][i] = data[4 * i + 2];
  }

  float* image_ptr[3];
  image_ptr[0] = &(images[2].at(0));  // B
  image_ptr[1] = &(images[1].at(0));  // G
  image_ptr[2] = &(images[0].at(0));  // R

  image.images = (unsigned char**)image_ptr;
  image.width = width;
  image.height = height;

  header.num_channels = 3;
  header.channels =
      (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);
  // Must be (A)BGR order, since most of EXR viewers expect this channel order.
  header.channels[0].name[0] = 'B';
  header.channels[0].name[1] = '\0';
  header.channels[1].name[0] = 'G';
  header.channels[1].name[1] = '\0';
  header.channels[2].name[0] = 'R';
  header.channels[2].name[1] = '\0';

  header.pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
  header.requested_pixel_types =
      (int*)malloc(sizeof(int) * header.num_channels);
  for (int i = 0; i < header.num_channels; i++) {
    header.pixel_types[i] =
        TINYEXR_PIXELTYPE_FLOAT;  // pixel type of input image
    header.requested_pixel_types[i] =
        TINYEXR_PIXELTYPE_FLOAT;  // pixel type of output image to be stored in
                                  // .EXR
  }

  const char* err = nullptr;
  int ret = SaveEXRImageToFile(&image, &header, path.c_str(), &err);
  if (ret != TINYEXR_SUCCESS) {
    std::cerr << "Save EXR err: " << err << std::endl;
    FreeEXRErrorMessage(err);  // free's buffer for an error message
    return ret;
  }
  std::cout << "Saved exr file. " << path << std::endl;

  free(header.channels);
  free(header.pixel_types);
  free(header.requested_pixel_types);
}

bool Init() {
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

  // Window非表示
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  // ウィンドウの作成
  window = glfwCreateWindow(64, 64, "", nullptr, nullptr);
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

  return true;
}

const GLuint CreateProgram(const std::string& vertex_shader_pass,
                           const std::string& fragment_shader_pass) {
  // 頂点シェーダの読み込み
  std::ifstream vertex_ifs(vertex_shader_pass, std::ios::binary);
  if (vertex_ifs.fail()) {
    std::cerr << "Error: Can't open source file: " << vertex_shader_pass
              << std::endl;
    return 0;
  }
  auto vertex_shader_source =
      std::string(std::istreambuf_iterator<char>(vertex_ifs),
                  std::istreambuf_iterator<char>());
  if (vertex_ifs.fail()) {
    std::cerr << "Error: could not read source file: " << vertex_shader_pass
              << std::endl;
    return 0;
  }
  GLchar const* vertex_shader_source_pointer = vertex_shader_source.c_str();

  // フラグメントシェーダの読み込み
  std::ifstream fragment_ifs(fragment_shader_pass, std::ios::binary);
  if (fragment_ifs.fail()) {
    std::cerr << "Error: Can't open source file: " << fragment_shader_pass
              << std::endl;
    return 0;
  }
  auto fragment_shader_source =
      std::string(std::istreambuf_iterator<char>(fragment_ifs),
                  std::istreambuf_iterator<char>());
  if (fragment_ifs.fail()) {
    std::cerr << "Error: could not read source file: " << fragment_shader_pass
              << std::endl;
    return 0;
  }
  GLchar const* fragment_shader_source_pointer = fragment_shader_source.c_str();

  // プログラムオブジェクトを作成
  const GLuint program = glCreateProgram();

  GLint status = GL_FALSE;
  GLsizei info_log_length;

  // 頂点シェーダのコンパイル
  const GLuint vertex_shader_obj = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader_obj, 1, &vertex_shader_source_pointer, nullptr);
  glCompileShader(vertex_shader_obj);
  glAttachShader(program, vertex_shader_obj);

  // 頂点シェーダのチェック
  glGetShaderiv(vertex_shader_obj, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
    std::cerr << "Compile Error in Vertex Shader." << std::endl;
  glGetShaderiv(vertex_shader_obj, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 1) {
    std::vector<GLchar> vertex_shader_error_message(info_log_length);
    glGetShaderInfoLog(vertex_shader_obj, info_log_length, nullptr,
                       vertex_shader_error_message.data());
    std::cerr << vertex_shader_error_message.data() << std::endl;
  }

  glDeleteShader(vertex_shader_obj);

  // フラグメントシェーダのコンパイル
  const GLuint fragment_shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_obj, 1, &fragment_shader_source_pointer,
                 nullptr);
  glCompileShader(fragment_shader_obj);
  glAttachShader(program, fragment_shader_obj);

  // フラグメントシェーダのチェック
  glGetShaderiv(fragment_shader_obj, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
    std::cerr << "Compile Error in Fragment Shader." << std::endl;
  glGetShaderiv(fragment_shader_obj, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 1) {
    std::vector<GLchar> fragment_shader_error_message(info_log_length);
    glGetShaderInfoLog(fragment_shader_obj, info_log_length, nullptr,
                       fragment_shader_error_message.data());
    std::cerr << fragment_shader_error_message.data() << std::endl;
  }

  glDeleteShader(fragment_shader_obj);

  // プログラムのリンク
  glLinkProgram(program);

  // リンクのチェック
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 1) {
    std::vector<GLchar> program_link_error_message(info_log_length);
    glGetProgramInfoLog(program, info_log_length, nullptr,
                        program_link_error_message.data());
    std::cerr << program_link_error_message.data() << std::endl;
  }

  return program;
}

/**
 * @brief LUTをベイクする
 */
void BakeLut(const std::string& output_path) {
  // textureの作成
  GLuint lut_texture;
  glGenTextures(1, &lut_texture);
  glBindTexture(GL_TEXTURE_2D, lut_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 256, 256, 0, GL_RGBA, GL_FLOAT,
               nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  // fboの作成
  GLuint lut_fbo;
  glGenFramebuffers(1, &lut_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, lut_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         lut_texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // fullscreen meshの作成
  GLuint fullscreen_mesh_vao;
  glGenVertexArrays(1, &fullscreen_mesh_vao);
  glBindVertexArray(fullscreen_mesh_vao);
  const std::array<glm::vec2, 3> fullscreen_mesh_vertices = {
      glm::vec2(-1.0, -1.0),
      glm::vec2(3.0, -1.0),
      glm::vec2(-1.0, 3.0),
  };
  GLuint fullscreen_mesh_vertices_vbo;
  glGenBuffers(1, &fullscreen_mesh_vertices_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, fullscreen_mesh_vertices_vbo);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2),
               &fullscreen_mesh_vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
  const std::array<glm::vec2, 3> fullscreen_mesh_uvs = {
      glm::vec2(0.0, 0.0), glm::vec2(2.0, 0.0), glm::vec2(0.0, 2.0)};
  GLuint fullscreen_mesh_uvs_vbo;
  glGenBuffers(1, &fullscreen_mesh_uvs_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, fullscreen_mesh_uvs_vbo);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2), &fullscreen_mesh_uvs[0],
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  // programの作成
  GLuint shader_program =
      CreateProgram("IBLSpecularShader/Lut.vert", "IBLSpecularShader/Lut.frag");

  // Bake
  glUseProgram(shader_program);
  glBindFramebuffer(GL_FRAMEBUFFER, lut_fbo);
  glViewport(0, 0, 256, 256);
  glBindVertexArray(fullscreen_mesh_vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  std::unique_ptr<float[]> data = std::make_unique<float[]>(4 * 256 * 256);
  glReadPixels(0, 0, 256, 256, GL_RGBA, GL_FLOAT, data.get());
  SaveExr(std::move(data), output_path, 256, 256);
}

/**
 * @brief キューブマップのPrefiltered Mapを生成する
 */
void BakePrefilteredMap(const std::string& input_path,
                        const float sky_intensity,
                        const std::string& output_path) {
  // skyテクスチャの読み込み
  const auto sky_exr = LoadExr(input_path);

  // 各種roughness用のtextureとfboの作成
  std::array<GLuint, 5> textures;
  std::array<GLuint, 5> fbos;
  for (int i = 0; i < 5; ++i) {
    glGenTextures(1, &textures[i]);
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 256 * std::pow(0.5, i),
                 256 * std::pow(0.5, i), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenFramebuffers(1, &fbos[i]);
    glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           textures[i], 0);
  }

  // fullscreen meshの作成
  GLuint fullscreen_mesh_vao;
  glGenVertexArrays(1, &fullscreen_mesh_vao);
  glBindVertexArray(fullscreen_mesh_vao);
  const std::array<glm::vec2, 3> fullscreen_mesh_vertices = {
      glm::vec2(-1.0, -1.0),
      glm::vec2(3.0, -1.0),
      glm::vec2(-1.0, 3.0),
  };
  GLuint fullscreen_mesh_vertices_vbo;
  glGenBuffers(1, &fullscreen_mesh_vertices_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, fullscreen_mesh_vertices_vbo);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2),
               &fullscreen_mesh_vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
  const std::array<glm::vec2, 3> fullscreen_mesh_uvs = {
      glm::vec2(0.0, 0.0), glm::vec2(2.0, 0.0), glm::vec2(0.0, 2.0)};
  GLuint fullscreen_mesh_uvs_vbo;
  glGenBuffers(1, &fullscreen_mesh_uvs_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, fullscreen_mesh_uvs_vbo);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2), &fullscreen_mesh_uvs[0],
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  // programの作成とuniformのlocationの取得
  const auto program = CreateProgram("IBLSpecularShader/shader.vert",
                                     "IBLSpecularShader/shader.frag");
  const auto rotation_loc = glGetUniformLocation(program, "Rotation");
  const auto sky_intensity_loc = glGetUniformLocation(program, "skyIntensity");
  const auto roughness_loc = glGetUniformLocation(program, "Roughness");

  // カメラのrotation
  const std::array<glm::mat4, 6> rotations = {
      glm::inverse(glm::lookAt(glm::vec3(0), glm::vec3(1.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f))),
      glm::inverse(glm::lookAt(glm::vec3(0), glm::vec3(-1.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f))),
      glm::inverse(glm::lookAt(glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f))),
      glm::inverse(glm::lookAt(glm::vec3(0), glm::vec3(0.0f, -1.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, -1.0f))),
      glm::inverse(glm::lookAt(glm::vec3(0), glm::vec3(0.0f, 0.0f, 1.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f))),
      glm::inverse(glm::lookAt(glm::vec3(0), glm::vec3(0.0f, 0.0f, -1.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)))};

  // 出力ファイル名
  const std::array<std::string, 5> output_dirs = {
      output_path + "/roughness-0",   output_path + "/roughness-0-25",
      output_path + "/roughness-0-5", output_path + "/roughness-0-75",
      output_path + "/roughness-1",
  };
  const std::array<std::string, 6> output_filename = {
      "pos-x.exr", "neg-x.exr", "pos-y.exr",
      "neg-y.exr", "pos-z.exr", "neg-z.exr",
  };

  // bake
  glUseProgram(program);
  glUniform1f(sky_intensity_loc, sky_intensity);
  for (int i = 0; i < 5; ++i) {
    std::filesystem::create_directories(output_dirs[i]);

    glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sky_exr);

    glUniform1f(roughness_loc, 0.25 * i);

    for (int j = 0; j < 6; ++j) {
      glUniformMatrix4fv(rotation_loc, 1, GL_FALSE, &rotations[j][0][0]);

      glViewport(0, 0, 256 * std::pow(0.5, i), 256 * std::pow(0.5, i));

      glBindVertexArray(fullscreen_mesh_vao);
      glDrawArrays(GL_TRIANGLES, 0, 3);

      std::unique_ptr<float[]> data = std::make_unique<float[]>(
          4 * 256 * std::pow(0.5, i) * 256 * std::pow(0.5, i));
      glReadPixels(0, 0, 256 * std::pow(0.5, i), 256 * std::pow(0.5, i),
                   GL_RGBA, GL_FLOAT, data.get());

      SaveExr(std::move(data), output_dirs[i] + "/" + output_filename[j],
              256 * std::pow(0.5, i), 256 * std::pow(0.5, i));
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc <= 3) {
    std::cerr << "src path, dst path and sky intensity is required." << std::endl;
    return -1;
  }

  Init();

  BakeLut(std::string(argv[3]) + "/Lut.exr");
  BakePrefilteredMap(std::string(argv[1]), std::stof(std::string(argv[2])),
                     std::string(argv[3]));
}
