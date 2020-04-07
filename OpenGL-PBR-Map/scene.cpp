#include "scene.h"

namespace game {

std::unique_ptr<Scene> Scene::CreateTestScene(const int width,
                                              const int height) {
  // Meshの読み込み
  auto mesh = Mesh::LoadObjMesh("monkey.obj");

  // Materialの作成
  auto material =
      std::make_shared<Material>(Texture("monkey_BaseColor.png", true),
                                 Texture("monkey_Metallic.png", false),
                                 Texture("monkey_Roughness.png", false),
                                 Texture("monkey_Normal.png", false),
                                 Texture("monkey_Emissive.png", true), 150.0f);

  // Sceneの作成
  auto scene = std::make_unique<Scene>();

  // Cameraの追加
  scene->camera_ = std::make_unique<Camera>(
      glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::radians(60.0f),
      static_cast<float>(width) / height, 0.1f, 100.0f);

  // MeshEntityの追加
  scene->mesh_entities_.emplace_back(mesh, material,
                                     glm::vec3(-2.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f), glm::vec3(1.0f));
  scene->mesh_entities_.emplace_back(mesh, material,
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f), glm::vec3(1.0f));
  scene->mesh_entities_.emplace_back(mesh, material,
                                     glm::vec3(2.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f), glm::vec3(1.0f));

  // DirectionalLightの設定
  scene->directional_light_ = std::make_unique<DirectionalLight>(
      700.0f, glm::vec3(-0.5f, -1.0f, -0.5f), glm::vec3(1.0f));

  return scene;
}

}  // namespace game
