#ifndef OPENGL_PBR_MAP_POINT_LIGHT_PASS_H_
#define OPENGL_PBR_MAP_POINT_LIGHT_PASS_H_

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "create_program.h"
#include "scene.h"

namespace game {

/**
 * @brief PointLighatパスを表現するクラス
 *
 * GBufferとシーンのPointLightの情報をもとに
 * PointLightのライティングを計算しHDRのカラーバッファに書き込みます。
 */
class PointLightPass {
 public:
  /**
   * @brief このパスをレンダリングする
   * @param scene レンダリングするシーン
   *
   * PointLightのライティングをHDRカラーバッファに書き出します。
   */
  void Render(const Scene& scene) const;

  /**
   * @brief コンストラクタ
   * @param hdr_color_fbo HDRのフレームバッファオブジェクト
   * @param gbuffer0 GBuffer0のテクスチャID
   * @param gbuffer1 GBuffer1のテクスチャID
   * @param gbuffer2 GBuffer2のテクスチャID
   * @param sphere_vao 球のMeshのVAO
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   */
  PointLightPass(const GLuint hdr_color_fbo, const GLuint gbuffer0,
                 const GLuint gbuffer1, const GLuint gbuffer2,
                 const GLuint sphere_vao, const GLuint width,
                 const GLuint height);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで確保したリソースを開放します。
   */
  ~PointLightPass();

 private:
  const GLuint width_;
  const GLuint height_;

  const GLuint shadow_map_size_;

  const GLuint hdr_fbo_;
  const GLuint gbuffer0_;
  const GLuint gbuffer1_;
  const GLuint gbuffer2_;
  const GLuint sphere_vao_;

  const GLuint shadow_map_;
  const GLuint shadow_map_fbo_;

  const GLuint shadow_pass_shader_program_;
  const GLuint shadow_pass_model_loc_;
  const GLuint shadow_pass_light_view_projection_matrices_loc_;
  const GLuint shadow_pass_world_light_pos_loc_;
  const GLuint shadow_pass_range_loc_;

  const GLuint stencil_pass_shader_program_;
  const GLuint stencil_pass_model_view_projection_loc_;

  const GLuint shader_program_;
  const GLuint model_view_projection_loc_;
  const GLuint world_light_position_loc_;
  const GLuint light_intensity_loc_;
  const GLuint light_color_loc_;
  const GLuint light_range_loc_;
  const GLuint world_camera_pos_loc_;
  const GLuint view_projection_i_loc_;
  const GLuint projection_params_loc_;
  const GLuint shadow_bias_loc_;
  const GLuint use_shadow_loc_;

  /**
   * @brief シャドウマップテクスチャを作成する
   * @return 生成したテクスチャのID
   */
  static const GLuint CreateShadowMap(const GLuint shadow_map_size);

  /**
   * @brief シャドウマップのFBOを生成する
   * @param shadow_map シャドウマップテクスチャのID
   * @return 生成したFBOのID
   */
  static const GLuint CreateShadowMapFbo(const GLuint shadow_map);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_POINT_LIGHT_PASS_H_