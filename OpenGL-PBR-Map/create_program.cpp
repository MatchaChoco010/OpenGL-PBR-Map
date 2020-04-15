#include "create_program.h"

namespace game {

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

GLuint CreateProgram(const std::string& vertex_shader_pass,
                     const std::string& geometry_shader_pass,
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

  // ジオメトリシェーダの読み込み
  std::ifstream geometry_ifs(geometry_shader_pass, std::ios::binary);
  if (geometry_ifs.fail()) {
    std::cerr << "Error: Can't open source file: " << geometry_shader_pass
              << std::endl;
    return 0;
  }
  auto geometry_shader_source =
      std::string(std::istreambuf_iterator<char>(geometry_ifs),
                  std::istreambuf_iterator<char>());
  if (geometry_ifs.fail()) {
    std::cerr << "Error: could not read source file: " << vertex_shader_pass
              << std::endl;
    return 0;
  }
  GLchar const* geometry_shader_source_pointer = geometry_shader_source.c_str();

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

  // ジオメトリシェーダのコンパイル
  const GLuint geometry_shader_obj = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(geometry_shader_obj, 1, &geometry_shader_source_pointer,
                 nullptr);
  glCompileShader(geometry_shader_obj);
  glAttachShader(program, geometry_shader_obj);

  // ジオメトリシェーダのチェック
  glGetShaderiv(geometry_shader_obj, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
    std::cerr << "Compile Error in Geometry Shader." << std::endl;
  glGetShaderiv(geometry_shader_obj, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 1) {
    std::vector<GLchar> geometry_shader_error_message(info_log_length);
    glGetShaderInfoLog(geometry_shader_obj, info_log_length, nullptr,
                       geometry_shader_error_message.data());
    std::cerr << geometry_shader_error_message.data() << std::endl;
  }

  glDeleteShader(geometry_shader_obj);

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

}  // namespace game