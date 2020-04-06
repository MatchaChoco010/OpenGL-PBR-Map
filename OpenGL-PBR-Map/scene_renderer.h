#ifndef OPENGL_PBR_MAP_SCENE_RENDERER_H_
#define OPENGL_PBR_MAP_SCENE_RENDERER_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "geometry_pass.h"
#include "scene.h"

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

  const GLuint gbuffer0_;       // rgb: albedo, a: metallic
  const GLuint gbuffer1_;       // rgb: emissive, a: depth
  const GLuint gbuffer2_;       // rgb: normal, a: roughenss
  const GLuint gbuffer_depth_;  // depth buffer
  const GLuint gbuffer_fbo_;

  GeometryPass geometry_pass_;

  /**
   * @brief リソースの開放をする
   */
  void Release();

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
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_SCENE_RENDERER_H_