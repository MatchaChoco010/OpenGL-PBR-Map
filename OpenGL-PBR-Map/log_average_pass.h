#ifndef OPENGL_PBR_MAP_LOG_AVERAGE_PASS_H_
#define OPENGL_PBR_MAP_LOG_AVERAGE_PASS_H_

#include <algorithm>

#include "create_program.h"

namespace game {

/**
 * @brief LogAverageパスを表現するクラス
 *
 * HDRカラーバッファの輝度LのLogAverageを計算します。
 */
class LogAveragePass {
 public:
  /**
   * @brief このパスをレンダリングする
   *
   * HDRバッファの輝度のLogを書き出します。
   */
  void Render();

  /**
   * @brief LLogAverageの値を取得する
   * @return LLogAverageの値
   *
   * Renderを呼んだあとに呼び出します。
   */
  const GLfloat GetLLogAverage() const;

  /**
   * @brief コンストラクタ
   * @param hdr_color_buffer HDRカラーバッファのテクスチャID
   * @param fullscreen_mesh_vao 画面を覆うメッシュのVAO
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   */
  LogAveragePass(const GLuint hdr_color_buffer,
                 const GLuint fullscreen_mesh_vao_, const GLuint width,
                 const GLuint height);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで生成したリソースを開放します。
   */
  ~LogAveragePass();

 private:
  const GLuint width_;
  const GLuint height_;

  const GLuint hdr_color_buffer_;
  const GLuint fullscreen_mesh_vao_;

  const GLuint log_average_buffer_;
  const GLuint log_average_fbo_;

  const GLuint shader_program_;

  GLfloat l_average_;

  /**
   * @brief LogAverageBufferテクスチャを生成する
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   * @return 生成したテクスチャのID
   */
  static const GLuint CreateLogAverageBuffer(const GLuint width,
                                             const GLuint height);

  /**
   * @brief LogAverageのFBOを生成する
   * @param log_average_buffer LogAverageバッファテクスチャのID
   * @return 生成したFBOのID
   */
  static const GLuint CreateLogAverageFbo(const GLuint log_average_buffer);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_LOG_AVERAGE_PASS_H_
