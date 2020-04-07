#ifndef OPENGL_PBR_MAP_SCENE_RENDERER_H_
#define OPENGL_PBR_MAP_SCENE_RENDERER_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>

#include "directional_light_pass.h"
#include "exposure_pass.h"
#include "geometry_pass.h"
#include "point_light_pass.h"
#include "scene.h"
#include "tonemapping_pass.h"

namespace game {

/**
 * @brief レンダラーのクラス
 *
 * シーンのレンダリングを行います。
 * 古典的なDeferredレンダリングで実装しています。
 * リソースの多重開放を防ぐためにコピー禁止です。
 */
class SceneRenderer {
 public:
  /**
   * @brief メインのレンダリング処理
   * @param scene レンダリングするシーン
   * @param delta_time 前フレームからの時間（秒）
   */
  void Render(const Scene& scene, const double delta_time);

  /**
   * @brief コンストラクタ
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   */
  SceneRenderer(const GLuint width, const GLuint height);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで生成したフレームバッファオブジェクトを開放します。
   */
  ~SceneRenderer();

  // コピー禁止
  SceneRenderer(const SceneRenderer&) = delete;
  SceneRenderer& operator=(const SceneRenderer&) = delete;

 private:
  const GLuint width_;
  const GLuint height_;

  const GLuint fullscreen_mesh_vao_;
  const GLuint fullscreen_mesh_vertices_vbo_;
  const GLuint fullscreen_mesh_uvs_vbo_;

  const GLuint sphere_vao_;
  const GLuint sphere_vertices_vbo_;
  const GLuint sphere_indices_ibo_;

  const GLuint gbuffer0_;       // rgb: albedo, a: metallic
  const GLuint gbuffer1_;       // rgb: emissive, a: depth
  const GLuint gbuffer2_;       // rgb: normal, a: roughenss
  const GLuint gbuffer_depth_;  // depth buffer
  const GLuint gbuffer_fbo_;

  const GLuint hdr_color_buffer_;
  const GLuint hdr_depth_buffer_;
  const GLuint hdr_fbo_;

  const GLuint exposured_color_buffer_;
  const GLuint exposured_fbo_;

  GeometryPass geometry_pass_;
  DirectionalLightPass directional_light_pass_;
  PointLightPass point_light_pass_;
  ExposurePass exposure_pass_;
  TonemappingPass tonemapping_pass_;

  /**
   * @brief リソースの開放をする
   */
  void Release();

  /**
   * @brief スクリーンを覆うメッシュのVAOを作成する
   * @return 作成したVAOのID
   */
  static const GLuint CreateFullscreenMeshVao();

  /**
   * @brief スクリーンを覆うメッシュの頂点座標のVBOを作成する
   * @param fullscreen_mesh_vao スクリーンを覆うメッシュのVAOのID
   * @return 作成したVBOのID
   */
  static const GLuint CreateFullscreenMeshVerticesVbo(
      const GLuint fullscreen_mesh_vao);

  /**
   * @brief スクリーンを覆うメッシュのUVのVBOを作成する
   * @param fullscreen_mesh_vao スクリーンを覆うメッシュのVAOのID
   * @return 作成したVBOのID
   */
  static const GLuint CreateFullscreenMeshUvsVbo(
      const GLuint fullscreen_mesh_vao);

  /**
   * @brief 球状のメッシュのVAOを作成する
   * @return 作成したVAOのID
   *
   * 作成した球状のメッシュはPunctual Lightの描画範囲を決めるのに使います。
   */
  static const GLuint CreateSphereMeshVao();

  /**
   * @brief 球状のメッシュのVBOを作成する
   * @return 作成したVBOのID
   */
  static const GLuint CreateSphereMeshVbo(const GLuint sphere_mesh_vao);

  /**
   * @brief 球状のメッシュのIBOを作成する
   * @return 作成したIBOのID
   */
  static const GLuint CreateSphereMeshIbo(const GLuint sphere_mesh_vao);

  /**
   * @brief GBuffer0のテクスチャを作成する
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   * @return 作成したGBuffer0テクスチャのID
   */
  static const GLuint CreateGBuffer0(const GLuint width, const GLuint height);

  /**
   * @brief GBuffer1のテクスチャを作成する
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   * @return 作成したGBuffer1テクスチャのID
   */
  static const GLuint CreateGBuffer1(const GLuint width, const GLuint height);

  /**
   * @brief GBuffer2のテクスチャを作成する
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   * @return 作成したGBuffer2テクスチャのID
   */
  static const GLuint CreateGBuffer2(const GLuint width, const GLuint height);

  /**
   * @brief GBufferのデプステクスチャを作成する
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   * @return 作成したGBufferのデプステクスチャのID
   */
  static const GLuint CreateGBufferDepth(const GLuint width,
                                         const GLuint height);

  /**
   * @brief GBufferのFBOを作成する
   * @param gbuffer0 GBuffer0のテクスチャID
   * @param gbuffer1 GBuffer1のテクスチャID
   * @param gbuffer2 GBuffer2のテクスチャID
   * @param gbuffer_depth GBuffer DepthのテクスチャID
   * @return 作成したFBOのID
   */
  static const GLuint CreateGBufferFbo(const GLuint gbuffer0,
                                       const GLuint gbuffer1,
                                       const GLuint gbuffer2,
                                       const GLuint gbuffer_depth);

  /**
   * @brief HDRカラーバッファのテクスチャを作成する
   * @return 作成したHDRカラーバッファのテクスチャのID
   */
  static const GLuint CreateHdrColorBuffer(const GLuint width,
                                           const GLuint height);

  /**
   * @brief HDRバッファのデプスバッファテクスチャを作成する
   * @return 作成したHDRデプスバッファテクスチャのID
   */
  static const GLuint CreateHdrDepthBuffer(const GLuint width,
                                           const GLuint height);

  /**
   * @brief HDRバッファのFBOを作成する
   * @return 作成したHDRバッファのFBOのID
   */
  static const GLuint CreateHdrFbo(const GLuint hdr_color_buffer,
                                   const GLuint hdr_depth_buffer);

  /**
   * @brief exposuredカラーバッファのテクスチャを作成する
   * @return 作成したexposuredカラーバッファのテクスチャのID
   */
  static const GLuint CreateExposuredColorBuffer(const GLuint width,
                                                 const GLuint height);

  /**
   * @brief exposuredカラーバッファのFBOを作成する
   * @param exposured_color_buffer バッファのテクスチャのID
   * @return 作成したexposuredカラーバッファのFBOのID
   */
  static const GLuint CreateExposuredFbo(const GLuint exposured_color_buffer);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_SCENE_RENDERER_H_