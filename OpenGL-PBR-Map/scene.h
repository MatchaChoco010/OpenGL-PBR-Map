#ifndef OPENGL_PBR_MAP_SCENE_H_
#define OPENGL_PBR_MAP_SCENE_H_

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <limits>
#include <memory>
#include <unordered_map>
#include <vector>

#include "camera.h"
#include "directional_light.h"
#include "mesh.h"
#include "mesh_entity.h"
#include "point_light.h"
#include "spot_light.h"
#include "exr_texture.h"

namespace game {

class Scene {
 public:
  std::unique_ptr<Camera> camera_;
  std::vector<MeshEntity> mesh_entities_;
  std::unique_ptr<DirectionalLight> directional_light_;
  std::vector<PointLight> point_lights_;
  std::vector<SpotLight> spot_lights_;
  std::unique_ptr<ExrTexture> sky_texture_;
  GLfloat sky_intensity_;

  /**
   * @brief 影ボリュームを再計算する
   */
  void RecaluculateDirectionalShadowVolume();

  /**
   * @brief 与えられたパスのシーンを読み込む
   * @param path シーンディレクトリのパス
   * @param width 画面の横幅
   * @param height 画面の縦
   * @return ロードされたシーン
   *
   * シーンディレクトリのパスは最後に`/`を含みません。
   */
  static std::unique_ptr<Scene> LoadScene(const std::string path,
                                          const GLuint width,
                                          const GLuint height);

 private:
  /**
   * @brief 文字列を与えられた文字で分割する
   * @param s 分割する文字列
   * @param delim 分割に使う文字
   * @return 分割された文字列のvector
   */
  static std::vector<std::string> SplitString(const std::string& s, char delim);

  /**
   * @brief Meshをパースする
   * @param mesh_texts シーンファイルの1つのMesh部分の行ごとに分割された文字列
   * @return パースされたメッシュ
   */
  static std::shared_ptr<Mesh> ParseMesh(
      const std::vector<std::string>& mesh_texts);
  /**
   * @brief Materialをパースし読み込む
   * @path シーンディレクトリのパス
   * @param material_texts
   * シーンファイルの一つのMaterialの行ごとに分割された文字列
   * @return パースされて読み込まれたMaterial
   */
  static std::shared_ptr<Material> ParseMaterial(
      const std::string& path, const std::vector<std::string>& material_texts);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_SCENE_H_