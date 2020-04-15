#ifndef OPENGL_PBR_MAP_DIRECTIONAL_LIGHT_H_
#define OPENGL_PBR_MAP_DIRECTIONAL_LIGHT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace game {

/**
 * @brief 平行光源を表現するオブジェクト
 *
 * 平行光源の各種プロパティを保持し、影の計算用の行列を扱うクラスです。
 * Directional Lightはシーンに一つのみ含まれます。
 */
class DirectionalLight {
 public:
  /**
   * @brief 光源の強さを取得する
   * @return 光源の強さ（lx）
   *
   * 平行光源の強さはlx（ルクス）で表現します。
   */
  const GLfloat GetIntensity() const;

  /**
   * @brief 光源の強さを設定する
   * @param intensity 新しい光源の強さ（lx）
   *
   * 平行光源の強さはlx（ルクス）で表現します。
   */
  void SetIntensity(const GLfloat intensity);

  /**
   * @brief 平行光源の向きを取得する
   * @return 平行光源の向きのベクトル
   *
   * 正規化された向きベクトルを返します。
   */
  const glm::vec3 GetDirection() const;

  /**
   * @brief 平行光源の向きを設定する
   * @param direction 新しい平行光源の向きのベクトル
   *
   * 渡されたベクトルは正規化され保持されます。
   * 内部で保持する影行列を再計算します。
   */
  void SetDirection(const glm::vec3 direction);

  /**
   * @brief 平行光源の色を取得する
   * @return 平行光源の色
   */
  const glm::vec3 GetColor() const;

  /**
   * @brief 平行光源の色を設定する
   * @param color 新しい平行光源の色
   */
  void SetColor(const glm::vec3 color);

  /**
   * @brief 影ボリュームを設定する
   * @param legt 影ボリュームのleftの値
   * @param right 影ボリュームのrightの値
   * @param bottom 影ボリュームのbottomの値
   * @param top 影ボリュームのtopの値
   * @param near 影ボリュームのnearの値
   * @param far 影ボリュームのfarの値
   * @param light_view ライトのビュー行列
   */
  void SetShadowVolume(const GLfloat left, const GLfloat right,
                       const GLfloat bottom, const GLfloat top,
                       const GLfloat near, const GLfloat far,
                       const glm::mat4 light_view);

  /**
   * @brief 影行列を取得する
   * @return 影行列
   */
  const glm::mat4 GetLightViewProjectionMatrix() const;

  /**
   * @brief コンストラクタ
   * @param intensity 光源の強さ（lx）
   * @param direction 光源の向きベクトル
   * @param color 光源の色
   */
  DirectionalLight(const GLfloat intensity, const glm::vec3 direction,
                   const glm::vec3 color);

 private:
  GLfloat intensity_;
  glm::vec3 direction_;
  glm::vec3 color_;
  glm::mat4 light_view_projection_matrix_;
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_DIRECTIONAL_LIGHT_H_
