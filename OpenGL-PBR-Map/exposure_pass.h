#ifndef OPENGL_PBR_MAP_EXPOSURE_PASS_H_
#define OPENGL_PBR_MAP_EXPOSURE_PASS_H_

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "create_program.h"

namespace game {

/**
 * @brief exposureパスを表現するクラス
 *
 * exposureを処理します。
 */
class ExposurePass final {
 public:
  /**
   * @brief このパスをレンダリングする
   *
   * exposureとexponential fogを処理します。
   */
  void Render() const;

  /**
   * @brief 露出の値を設定します
   * @param exposure 露出の値
   */
  void SetExposure(const GLfloat exposure);

  /**
   * @brief コンストラクタ
   * @param hdr_color_buffer HDRカラーバッファテクスチャのID
   * @param exposured_fbo Exposed FBOのID
   * @param fullscreen_mesh_vao 画面を覆うメッシュのVAO
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   */
  ExposurePass(const GLuint hdr_color_buffer, const GLuint exposured_fbo,
               const GLuint fullscreen_mesh_vao, const GLuint width,
               const GLuint height);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで生成したシェーダプログラム及びFBO、テクスチャを破棄します。
   */
  ~ExposurePass();

 private:
  const GLuint width_;
  const GLuint height_;

  const GLuint hdr_color_buffer_;
  const GLuint exposured_fbo_;
  const GLuint fullscreen_mesh_vao_;

  const GLuint shader_program_;
  const GLuint exposure_loc_;

  GLfloat exposure_;
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_EXPOSURE_PASS_H_