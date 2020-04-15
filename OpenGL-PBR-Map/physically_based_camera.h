#ifndef OPENGL_PBR_MAP_PHYSICALLY_BASED_CAMERA_H_
#define OPENGL_PBR_MAP_PHYSICALLY_BASED_CAMERA_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace game {

/**
 * @brief Physically Based Cameraのクラス
 *
 * 物理ベースの露出補正を行うカメラのクラスです。
 * Frostbiteの2014年のSIGGRAPHの資料をベースにした次の記事を参考にしました。
 * https://placeholderart.wordpress.com/2014/11/16/implementing-a-physically-based-camera-understanding-exposure/
 * https://placeholderart.wordpress.com/2014/11/21/implementing-a-physically-based-camera-manual-exposure/
 * https://placeholderart.wordpress.com/2014/12/15/implementing-a-physically-based-camera-automatic-exposure/
 */
class PhysicallyBasedCamera final {
 public:
  /**
   * @brief カメラの露出を更新する
   * @param l_new 画面輝度の対数平均値
   * @param delta_time 前フレームからの時間（秒）
   */
  void Update(const GLfloat l_new, const double delta_time);

  /**
   * @brief ISOの値を取得する
   * @return ISOの値
   */
  const GLfloat GetIso() const;

  /**
   * @brief 絞りの値を取得する
   * @return 絞りの値
   */
  const GLfloat GetAperture() const;

  /**
   * @brief シャッタースピードを取得する
   * @return シャッタースピード（秒）
   */
  const GLfloat GetShutterSpeed() const;

  /**
   * @brief EVcompの値を取得する
   * @return EVcompの値
   */
  const GLfloat GetEvComop() const;

  /**
   * @brief EVcompの値を設定する
   * @param ev_comp 新しいEVcompの値
   */
  void SetEvComp(const GLfloat ev_comp);

  /**
   * @brief 露出を取得する
   * @return 露出の値
   */
  const GLfloat GetExposure() const;

  /**
   * @brief コンストラクタ
   * @param l_avg 画面平均輝度の初期値
   * @param ev_comp EVcompの初期値
   */
  PhysicallyBasedCamera(const GLfloat l_avg, const GLfloat ev_comp);

 private:
  static constexpr GLfloat kMinIso = 100.0f;
  static constexpr GLfloat kMaxIso = 6400.0f;
  static constexpr GLfloat kMinAperture = 1.8f;
  static constexpr GLfloat kMaxAperture = 22.0f;
  static constexpr GLfloat kMinShutterSpeed = 1.0f / 4000.0f;
  static constexpr GLfloat kMaxShutterSpeed = 1.0f / 30.0f;

  GLfloat l_avg_;
  GLfloat iso_;
  GLfloat aperture_;
  GLfloat shutter_speed_;
  GLfloat ev_comp_;
  GLfloat exposure_;

  /**
   * @brief TargetEVを計算する
   * @return TargetEVの値
   */
  const GLfloat ComputeTargetEv(const GLfloat average_luminance);

  /**
   * @brief EVに合わせたISOの値を計算する
   * @param aperture 絞りの値
   * @param shutter_speed シャッタースピード（秒）
   * @param ev ターゲットとなるEVの値
   * @return 計算されたISOの値
   */
  const GLfloat ComputeIso(const GLfloat aperture, const GLfloat shutter_speed,
                           const GLfloat ev);

  /**
   * @brief EVの値を計算する
   * @param aperture 絞りの値
   * @param shutter_speed シャッタースピード（秒）
   * @param iso ISOの値
   * @return 計算されたEVの値
   */
  const GLfloat ComputeEv(const GLfloat aperture, const GLfloat shutter_speed,
                          const GLfloat iso);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_PHYSICALLY_BASED_CAMERA_H_