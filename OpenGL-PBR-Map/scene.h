#ifndef OPENGL_PBR_MAP_SCENE_H_
#define OPENGL_PBR_MAP_SCENE_H_

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "camera.h"
#include "directional_light.h"
#include "mesh.h"
#include "mesh_entity.h"
#include "point_light.h"

namespace game {

class Scene {
 public:
  std::unique_ptr<Camera> camera_;
  std::vector<MeshEntity> mesh_entities_;
  std::unique_ptr<DirectionalLight> directional_light_;
  std::vector<PointLight> point_lights_;

  /**
   * @brief テスト用のシーンを作成する
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   * @return テスト用シーン
   */
  static std::unique_ptr<Scene> CreateTestScene(const int width,
                                                const int height);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_SCENE_H_