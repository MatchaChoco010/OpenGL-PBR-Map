#ifndef OPENGL_PBR_MAP_CAMERA_H_
#define OPENGL_PBR_MAP_CAMERA_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace game {

/**
 * @brief カメラを表現するクラス
 */
class Camera {
 public:
  /**
   * @brief カメラの位置を取得する
   * @return カメラの位置
   */
  const glm::vec3 GetPosition() const;

  /**
   * @brief カメラの位置を設定する
   * @param position 新しい位置
   *
   * 内部で保持するView行列の再計算が行われます。
   */
  void SetPosition(const glm::vec3 position);

  /**
   * @brief カメラの回転のオイラー角を取得する
   * @return カメラの回転のオイラー角
   *
   * オイラー角はYXZの順です。
   */
  const glm::vec3 GetRotation() const;

  /**
   * @brief カメラの回転のオイラー角を設定する
   * @param rotation 新しい回転のオイラー角
   *
   * オイラー角はYXZの順です。
   * 内部で保持するView行列の再計算が行われます。
   */
  void SetRotation(const glm::vec3 rotation);

  /**
   * @brief カメラのProjection行列のパラメータを設定する
   * @param fovy fovyの値（radians）
   * @param aspect aspectの値
   * @param near nearの値
   * @param far farの値
   */
  void Perspective(const GLfloat fovy, const GLfloat aspect, const GLfloat near,
                   const GLfloat far);

  /**
   * @brief ViewProjection行列を計算し取得する
   * @return 計算されたViewProjection行列
   *
   * 呼び出すたびにview_matrix_とprojection_matrix_の掛け算が行われます。
   */
  const glm::mat4 GetViewProjectionMatrix() const;

  /**
   * @brief View行列を取得する
   * @return View行列
   */
  const glm::mat4 GetViewMatrix() const;

  /**
   * @brief Projection行列を取得する
   * @return Projection行列
   */
  const glm::mat4 GetProjectionMatrix() const;

  /**
   * @brief コンストラクタ
   * @param position カメラの位置
   * @param rotation カメラの回転のオイラー角
   * @param fovy fovyの値（radians）
   * @param aspect aspectの値
   * @param near nearの値
   * @param far farの値
   *
   * 必要なパラメータをすべて受け取るコンストラクタです。
   * オイラー角はYXZの順です。
   */
  Camera(const glm::vec3 position, const glm::vec3 rotation, const GLfloat fovy,
         const GLfloat aspect, const GLfloat near, const GLfloat far);

 private:
  glm::vec3 position_;
  glm::vec3 rotation_;
  glm::mat4 view_matrix_;
  glm::mat4 projection_matrix_;

  /**
   * @brief View行列を再計算する
   *
   * view_matrix_の値をposition_とrotation_から計算した値に設定します。
   */
  void RecaluculateViewMatirx();
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_CAMERA_H_