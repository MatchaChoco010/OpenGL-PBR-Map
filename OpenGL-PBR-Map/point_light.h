#ifndef OPENGL_PBR_MAP_POINT_LIGHT_H_
#define OPENGL_PBR_MAP_POINT_LIGHT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace game {

/**
 * @brief ポイントライトを表現するオブジェクト
 *
 * ポイントライトの各種プロパティを保持し、影行列を扱うクラスです。
 */
class PointLight {
 public:
  /**
   * @brief ポイントライトの位置を取得する
   * @return ポイントライトの位置
   */
  const glm::vec3 GetPosition() const;

  /**
   * @brief ポイントライトの位置を設定する
   * @param position ポイントライトの新しい位置
   *
   * 内部で保持する影行列とModel行列の再計算が行われます。
   */
  void SetPosition(const glm::vec3 position);

  /**
   * @brief ポイントライトの強さを取得する
   * @return ポイントライトの強さ（lm）
   *
   * ポイントライトの強さはlm（ルーメン）で表現します。
   */
  const GLfloat GetIntensity() const;

  /**
   * @brief ポイントライトの強さを設定する
   * @param intensity ポイントライトの新しい強さ
   *
   * ポイントライトの強さはlm（ルーメン）で表現します。
   */
  void SetIntensity(const GLfloat intensity);

  /**
   * @brief ポイントライトの色を取得する
   * @return ポイントライトの色
   */
  const glm::vec3 GetColor() const;

  /**
   * @brief ポイントライトの色を設定する
   * @param color ポイントライトの新しい色
   */
  void SetColor(const glm::vec3 color);

  /**
   * @brief ポイントライトのRangeを取得する
   * @return ポイントライトのRange（m）
   */
  const GLfloat GetRange() const;

  /**
   * @brief ポイントライトのRangeを設定する
   * @param range ポイントライトの新しいRange（m）
   */
  void SetRange(const GLfloat range);

  /**
   * @brief ポイントライトのModel行列を取得する
   * @return ポイントライトのModel行列
   *
   * range_ + 0.1fの拡大も含みます。
   */
  const glm::mat4 GetModelMatrix() const;

  /**
   * @brief コンストラクタ
   * @param position ポイントライトの位置
   * @param intensity ポイントライトの強さ（lx）
   * @param color ポイントライトの色
   * @param range ポイントライトのRange（m）
   */
  PointLight(const glm::vec3 position, const GLfloat intensity,
             const glm::vec3 color, const GLfloat range);

 private:
  glm::vec3 position_;
  GLfloat intensity_;
  glm::vec3 color_;
  GLfloat range_;
  glm::mat4 model_matrix_;

  /**
   * @brief Model行列を再計算する
   *
   * position_からmodel_matrix_を再計算します。
   */
  void RecaluculateModelMatrix();
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_POINT_LIGHT_H_
