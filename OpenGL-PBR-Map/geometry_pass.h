#ifndef OPENGL_PBR_MAP_GEOMETRY_PASS_H_
#define OPENGL_PBR_MAP_GEOMETRY_PASS_H_

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "create_program.h"
#include "scene.h"

namespace game {

class GeometryPass {
 public:
  /**
   * @brief このパスをレンダリングする
   * @param scene レンダリングするシーン
   */
  void Render(const Scene& sccene);

  /**
   * @brief コンストラクタ
   * @param gbuffer_fbo GBufferのFBO
   */
  GeometryPass(const GLuint gbuffer_fbo);

  /**
   * @brief デストラクタ
   *
   * コンストラクタで確保したシェーダプログラムを開放します。
   */
  ~GeometryPass();

 private:
  const GLuint gbuffer_fbo_;

  const GLuint shader_program_;

  const GLuint model_loc_;
  const GLuint model_it_loc_;
  const GLuint view_loc_;
  const GLuint projection_loc_;
  const GLuint projection_params_loc_;
  const GLuint emissive_intensity_loc_;
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_GEOMETRY_PASS_H_