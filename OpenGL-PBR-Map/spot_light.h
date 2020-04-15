#ifndef OPENGL_PBR_MAP_SPOT_LIGHT_H_
#define OPENGL_PBR_MAP_SPOT_LIGHT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace game {

/**
 * @brief スポットライトを表現するオブジェクト
 */
class SpotLight {
 public:
  /**
   * @brief スポットライトの位置を取得する
   * @return スポットライトの位置
   */
  const glm::vec3 GetPosition() const;

  /**
   * @brief スポットライトの位置を設定する
   * @param position スポットライトの新しい位置
   *
   * 内部で保持する影行列とModel行列の再計算が行われます。
   */
  void SetPosition(const glm::vec3 position);

  /**
   * @brief スポットライトの強さを取得する
   * @return スポットライトの強さ（lm）
   *
   * スポットライトの強さはlm（ルーメン）で表現します。
   */
  const GLfloat GetIntensity() const;

  /**
   * @brief スポットライトの強さを設定する
   * @param intensity スポットライトの新しい強さ
   *
   * スポットライトの強さはlm（ルーメン）で表現します。
   */
  void SetIntensity(const GLfloat intensity);

  /**
   * @brief スポットライトの色を取得する
   * @return スポットライトの色
   */
  const glm::vec3 GetColor() const;

  /**
   * @brief スポットライトの色を設定する
   * @param color スポットライトの新しい色
   */
  void SetColor(const glm::vec3 color);

  /**
   * @brief スポットライトの影のnearを取得する
   * @return スポットライトの影のnearの値
   */
  const GLfloat GetNear() const;

  /**
   * @brief スポットライトの影のnearの値を設定する
   * @param near スポットライトの影の新しいnearの値
   *
   * 内部で保持する影行列の再計算が行われます。
   */
  void SetNear(const GLfloat near);

  /**
   * @brief スポットライトのRangeを取得する
   * @return スポットライトのRange（m）
   */
  const GLfloat GetRange() const;

  /**
   * @brief スポットライトのRangeを設定する
   * @param range スポットライトの新しいRange（m）
   *
   * 内部で保持する影行列とModel行列の再計算が行われます。
   */
  void SetRange(const GLfloat range);

  /**
   * @brief スポットライトの向きを取得する
   * @return スポットライトの向きのベクトル
   *
   * 正規化された向きベクトルを返します。
   */
  const glm::vec3 GetDirection() const;

  /**
   * @brief スポットライトの向きを設定する
   * @param direction 新しいスポットライトの向きのベクトル
   *
   * 渡されたベクトルは正規化され保持されます。
   * 内部で保持する影行列を再計算します。
   */
  void SetDirection(const glm::vec3 direction);

  /**
   * @brief スポットライトのAngleを取得する
   * return スポットライトのAngle（radians）
   */
  const GLfloat GetAngle() const;

  /**
   * @brief スポットライトのAngleを設定する
   * @param angle スポットライトのAngle（radians）
   *
   * 内部で保持する影行列を再計算します。
   */
  void SetAngle(const GLfloat angle);

  /**
   * @brief スポットライトの半影の割合を取得する
   * @return スポットライトの半影の割合（[0,1]）
   *
   * スポットライトの半影と本影のブレンド率を取得します。
   * 0で本影のみ、1で半影のみとなります。
   */
  const GLfloat GetBlend() const;

  /**
   * @brief スポットライトの半影の割合を設定する
   * @param blend スポットライトの半影の割合（[0,1]）
   *
   * スポットライトの半影と本影のブレンド率を取得します。
   * 0で本影のみ、1で半影のみとなります。
   */
  void SetBlend(const GLfloat blend);

  /**
   * @brief 影行列を取得する
   * @return 影行列
   */
  const glm::mat4 GetLightViewProjectionMatrix() const;

  /**
   * @brief Model行列を取得する
   * @return Model行列
   *
   * range_ + 0.1fの拡大も含みます。
   */
  const glm::mat4 GetModelMatrix() const;

  /**
   * @brief コンストラクタ
   * @param position スポットライトの位置
   * @param intensity スポットライトの強さ（lm）
   * @param color スポットライトの色
   * @param near スポットライトの影のnearの値
   * @param range スポットライトのRangeの値
   * @param direction スポットライトの向きのベクトル
   * @param angle スポットライトのAngle（radians）
   * @param blend スポットライトの半影の割合（[0,1]）
   */
  SpotLight(const glm::vec3 position, const GLfloat intensity,
            const glm::vec3 color, const GLfloat near, const GLfloat range,
            const glm::vec3 direction, const GLfloat angle,
            const GLfloat blend);

 private:
  glm::vec3 position_;
  GLfloat intensity_;
  glm::vec3 color_;
  GLfloat near_;
  GLfloat range_;
  glm::vec3 direction_;
  GLfloat angle_;
  GLfloat blend_;
  glm::mat4 light_view_projection_matrix_;
  glm::mat4 model_matrix_;

  /**
   * @brief 影行列を再計算する
   *
   * position_、direction_、range_、angle_から
   * light_view_projection_matrix_を再計算します。
   */
  void RecaluculateLightViewProjectionMatrix();

  /**
   * @brief Model行列を再計算する
   *
   * position_からmodel_matrix_を再計算します。
   */
  void RecaluculateModelMatrix();
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_SPOT_LIGHT_H_