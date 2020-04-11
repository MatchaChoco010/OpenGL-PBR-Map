#ifndef OPENGL_PBR_MAP_CREATE_PROGRAM_H_
#define OPENGL_PBR_MAP_CREATE_PROGRAM_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace game {

/**
 * @brief シェーダを読み込む
 * @param vertex_shader_pass 頂点シェーダのパス
 * @param fragment_shader_pass フラグメントシェーダのパス
 */
const GLuint CreateProgram(const std::string& vertex_shader_pass,
                           const std::string& fragment_shader_pass);

/**
 * @brief シェーダを読み込む
 * @param vertex_shader_pass 頂点シェーダのパス
 * @param geometry_shader_pass ジオメトリシェーダのパス
 * @param fragment_shader_pass フラグメントシェーダのパス
 */
GLuint CreateProgram(const std::string& vertex_shader_pass,
                     const std::string& geometry_shader_pass,
                     const std::string& fragment_shader_pass);

}  // namespace game

#endif  // OPENGL_PBR_MAP_CREATE_PROGRAM_H_