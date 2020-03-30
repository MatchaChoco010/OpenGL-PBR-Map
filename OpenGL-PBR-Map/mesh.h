#ifndef OPENGL_PBR_MAP_MESH_H_
#define OPENGL_PBR_MAP_MESH_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace game {

class Mesh {
 public:
  /**
   * @brief VAOをバインドし描画する
   */
  void Draw() const;

  /**
   * @brief コンストラクタ
   * @param vertices 頂点位置の列
   * @param colors 頂点色の列
   *
   * ジオメトリを構築し、VBOとVAOを構築します。
   * 各種頂点情報は前から順に3つずつで一つの面を構成していきます。
   */
  Mesh(const std::vector<glm::vec3>& vertices,
       const std::vector<glm::vec3>& colors);

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
   * @brief 三角形メッシュを作成する静的メンバ関数
   * @return 三角形メッシュ
   */
  static std::unique_ptr<Mesh> CreateTriangleMesh();

 private:
  GLuint size_;
  GLuint vertices_vbo_;
  GLuint colors_vbo_;
  GLuint vao_;

  /**
   * @brief OpenGLのオブジェクトを開放する
   *
   * コンストラクタで確保したVAOとVBOを開放します。
   */
  void Release();
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_MESH_H_