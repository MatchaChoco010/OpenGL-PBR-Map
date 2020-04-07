#ifndef OPENGL_PBR_MAP_TONEMAPPING_PASS_H_
#define OPENGL_PBR_MAP_TONEMAPPING_PASS_H_

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "create_program.h"

namespace game {

/**
 * @brief トーンマッピングパスを表現するクラス
 *
 * 露出補正とACESトーンマッピング、ガンマ補正を行うパスです。
 * ACESの式は次のオリジナルのGitHubコードをGLSLに書き換えました。
 * https://github.com/ampas/aces-dev
 */
class TonemappingPass {
 public:
  /**
   * @brief このパスをレンダリングする
   *
   * 露出補正とACESトーンマッピングを行います。
   */
  void Render() const;

  /**
   * @brief コンストラクタ
   * @param hdr_color_buffer HDRカラーバッファテクスチャのID
   * @param fullscreen_mesh_vao 画面を覆うメッシュのVAO
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   *
   * シェーダプログラムを作成します。
   * HDRカラーバッファテクスチャ及びfullscreen VAOの所有権は奪いません。
   * HDRカラーバッファテクスチャ及びfullscreen
   * VAOの開放の責任は外側のスコープです。
   */
  TonemappingPass(const GLuint hdr_color_buffer,
                  const GLuint fullscreen_mesh_vao, const GLuint width,
                  const GLuint height);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで生成したシェーダプログラムを破棄します。
   */
  ~TonemappingPass();

 private:
  const GLuint exposured_color_buffer_;
  const GLuint fullscreen_mesh_vao_;

  const GLuint shader_program_;
};

}

#endif  // OPENGL_PBR_MAP_TONEMAPPING_PASS_H_