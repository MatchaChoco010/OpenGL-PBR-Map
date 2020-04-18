#ifndef OPENGL_PBR_MAP_SPECULAR_IBL_PASS_PASS_H_
#define OPENGL_PBR_MAP_SPECULAR_IBL_PASS_PASS_H_

#include "create_program.h"
#include "scene.h"

namespace game {

class SpecularIblPass {
 public:
  /**
   * @brief このパスをレンダリングする
   *
   * HDRバッファの輝度のLogを書き出します。
   */
  void Render(const Scene& scene) const;

  /**
   * @brief コンストラクタ
   * @param hdr_fbo HDRのフレームバッファオブジェクト
   * @param gbuffer0 GBuffer0のテクスチャID
   * @param gbuffer1 GBuffer1のテクスチャID
   * @param gbuffer2 GBuffer2のテクスチャID
   * @param fullscreen_vao 画面を覆うメッシュのVAO
   */
  SpecularIblPass(const GLuint hdr_fbo, const GLuint gbuffer0,
                 const GLuint gbuffer1, const GLuint gbuffer2,
                 const GLuint fullscreen_vao);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで生成したリソースを開放します。
   */
  ~SpecularIblPass();

 private:
  const GLuint hdr_fbo_;
  const GLuint gbuffer0_;
  const GLuint gbuffer1_;
  const GLuint gbuffer2_;
  const GLuint fullscreen_vao_;

  const GLuint shader_program_;
  const GLuint world_camera_pos_loc_;
  const GLuint view_projection_i_loc_;
  const GLuint projection_params_loc_;
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_SPECULAR_IBL_PASS_PASS_H_