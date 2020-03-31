#ifndef OPENGL_PBR_MAP_MESH_H_
#define OPENGL_PBR_MAP_MESH_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace game {

/**
 * @brief Meshを表現するクラス
 *
 * 多重開放を避けるためコピー禁止です。
 */
class Mesh {
 public:
  /**
   * @brief VAOをバインドし描画する
   */
  void Draw() const;

  /**
   * @brief コンストラクタ
   * @param vertices 頂点位置の列
   * @param normals 頂点法線の列
   *
   * ジオメトリを構築し、VBOとVAOを構築します。
   * 各種頂点情報は前から順に3つずつで一つの面を構成していきます。
   */
  Mesh(const std::vector<glm::vec3>& vertices,
       const std::vector<glm::vec3>& normals);

  /**
   * @brief デストラクタ
   *
   * VBOとVAOを開放します。
   */
  ~Mesh();

  // コピー禁止
  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  /**
   * @brief ムーブコンストラクタ
   * @param other ムーブ元
   *
   * ムーブ後はVAO及びVBOは0に設定されます。
   */
  Mesh(Mesh&& other) noexcept;

  /**
   * @brief ムーブ代入演算子
   * @param other ムーブ元
   *
   * ムーブ後はVAO及びVBOは0に設定されます。
   */
  Mesh& operator=(Mesh&& other) noexcept;

  /**
   * @brief objを読み込む静的メンバ関数
   * @param file 読み込むobjファイルのパス
   * @return 読み込んだMesh
   */
  static std::shared_ptr<Mesh> LoadObjMesh(const std::string file);

 private:
  GLuint size_;
  GLuint vertices_vbo_;
  GLuint normals_vbo_;
  GLuint vao_;

  /**
   * @brief OpenGLのオブジェクトを開放する
   *
   * コンストラクタで確保したVAOとVBOを開放します。
   */
  void Release();

  /**
   * @brief 文字列を指定した区切り文字で分割する静的メンバ関数
   * @param s 分割する文字列
   * @param delim 分割に使う区切り文字
   * @return 分割された文字列
   */
  static std::vector<std::string> SplitString(const std::string& s, char delim);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_MESH_H_