#ifndef OPENGL_PBR_MAP_MESH_ENTITY_H_
#define OPENGL_PBR_MAP_MESH_ENTITY_H_

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <memory>

#include "mesh.h"

namespace game {

/**
 * @brief MeshとModel行列を保持するクラス
 */
class MeshEntity {
 public:
  const std::shared_ptr<const Mesh> mesh_;

  /**
   * @brief シーン上の位置を返す
   * @return シーン上の位置
   */
  const glm::vec3 GetPosition() const;

  /**
   * @brief シーン上の位置を設定する
   * @param position 新しい位置
   *
   * 内部で保持するModel行列の再計算が行われます。
   */
  void SetPosition(const glm::vec3 position);

  /**
   * @brief このEntityの回転のオイラー角を取得する
   * @return このEntityの回転のオイラー角
   *
   * オイラー角はYXZの順です。
   */
  const glm::vec3 GetRotation() const;

  /**
   * @brief このEntityの回転のオイラー角を設定する
   * @param rotation 新しい回転のオイラー角
   *
   * オイラー角はYXZの順です。
   * 内部で保持するModel行列の再計算が行われます。
   */
  void SetRotation(const glm::vec3 rotation);

  /**
   * @brief このEntityのスケールを取得する
   * @return このEntityのスケール
   */
  const glm::vec3 GetScale() const;

  /**
   * @brief このEntityのスケールを設定する
   * @param scale 新しいスケール
   *
   * 内部で保持するModel行列の再計算が行われます。
   */
  void SetScale(const glm::vec3 scale);

  /**
   * @brief Model行列を取得する
   * @return Model行列
   */
  const glm::mat4 GetModelMatrix() const;

  /**
   * @brief コンストラクタ
   * @param mesh Meshのshared_ptr
   * @param material Materialのshared_ptr
   * @param position Entityの位置
   * @param rotation Entityの回転のオイラー角
   * @param scale Entityの各軸のスケール
   *
   * オイラー角はYXZの順です。
   */
  MeshEntity(const std::shared_ptr<const Mesh> mesh, const glm::vec3 position,
             const glm::vec3 rotation, const glm::vec3 scale);

 private:
  glm::vec3 position_;
  glm::vec3 rotation_;
  glm::vec3 scale_;
  glm::mat4 model_matrix_;

  /**
   * @brief Model行列を再計算する
   */
  void RecaluculateModelMatrix();
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_MESH_ENTITY_H_