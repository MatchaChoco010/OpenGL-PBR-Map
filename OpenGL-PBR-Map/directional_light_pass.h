#ifndef OPENGL_PBR_MAP_DIRECTIONAL_LIGHT_PASS_H_
#define OPENGL_PBR_MAP_DIRECTIONAL_LIGHT_PASS_H_

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "create_program.h"
#include "scene.h"

namespace game {

/**
 * @brief DirectionalLighatパスを表現するクラス
 *
 * GBufferとシーンのDirectionalLightの情報をもとに
 * DirectionalLightのライティングを計算しHDRのカラーバッファに書き込みます。
 * emissiveの値も処理します。
 * リソースの多重開放を避けるためコピー禁止です。
 */
class DirectionalLightPass {
 public:
  /**
   * @brief このパスをレンダリングする
   * @param scene レンダリングするシーン
   *
   * DirectionalLightのライティングとemissive項をHDRカラーバッファに書き出します。
   */
  void Render(const Scene& scene) const;

  /**
   * @brief コンストラクタ
   * @param hdr_fbo HDRのフレームバッファオブジェクト
   * @param gbuffer0 GBuffer0のテクスチャID
   * @param gbuffer1 GBuffer1のテクスチャID
   * @param gbuffer2 GBuffer2のテクスチャID
   * @param fullscreen_vao 画面を覆むメッシュのVAO
   * @param irradiance_map 放射照度マップのテクスチャID
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   *
   * FBO及びGBufferテクスチャ、VAOの所有権は奪いません。
   * FBO及びGBufferテクスチャ、VAOの開放の責任は外側のスコープです。
   * シャドウマップ用のFBOも確保します。
   */
  DirectionalLightPass(const GLuint hdr_fbo, const GLuint gbuffer0,
                       const GLuint gbuffer1, const GLuint gbuffer2,
                       const GLuint fullscreen_vao, const GLuint width,
                       const GLuint height);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで確保したリソースを開放します。
   */
  ~DirectionalLightPass();

 private:
  const GLuint width_;
  const GLuint height_;

  const GLuint shadow_map_size_;

  const GLuint hdr_fbo_;
  const GLuint gbuffer0_;
  const GLuint gbuffer1_;
  const GLuint gbuffer2_;
  const GLuint fullscreen_vao_;

  const GLuint shadow_map_;
  const GLuint shadow_map_fbo_;

  const GLuint shadow_pass_shader_program_;
  const GLuint shadow_pass_model_view_projection_loc_;

  const GLuint shader_program_;
  const GLuint light_direction_loc_;
  const GLuint light_intensity_loc_;
  const GLuint light_color_loc_;
  const GLuint world_camera_pos_loc_;
  const GLuint view_projection_i_loc_;
  const GLuint projection_params_loc_;
  const GLuint light_view_projection_loc_;

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

#endif  // OPENGL_PBR_MAP_DIRECTIONAL_LIGHT_PASS_H_