#ifndef OPENGL_PBR_MAP_SKY_PASS_H_
#define OPENGL_PBR_MAP_SKY_PASS_H_

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "create_program.h"
#include "scene.h"

namespace game {

/**
 * @brief 空の描画のパスを表すクラス
 *
 * HDRカラーバッファのカラーを空の色で塗りつぶします。
 */
class SkyPass {
 public:
  /**
   * @brief このパスをレンダリングする
   * @param scene レンダリングするシーン
   */
  void Render(const Scene& sccene) const;

  /**
   * @brief コンストラクタ
   * @param hdr_color_fbo
   * @param fullscreen_mesh_vao 全画面を覆うメッシュのVAO
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   */
  SkyPass(const GLuint hdr_color_fbo, const GLuint fullscreen_mesh_vao,
          const GLuint width, const GLuint height);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで確保したリソースを開放します。
   */
  ~SkyPass();

 private:
  const GLuint width_;
  const GLuint height_;

  const GLuint hdr_color_fbo_;

  const GLuint fullscreen_mesh_vao_;

  const GLuint shader_program_;
  const GLuint y_tan_loc_;
  const GLuint x_tan_loc_;
  const GLuint camera_rotation_matrix_loc_;
  const GLuint sky_intensity_loc_;
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_SKY_PASS_H_