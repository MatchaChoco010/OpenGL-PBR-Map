#include "tonemapping_pass.h"

namespace game {

void TonemappingPass::Render() const {
  glUseProgram(shader_program_);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, exposured_color_buffer_);

  glBindVertexArray(fullscreen_mesh_vao_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

TonemappingPass::TonemappingPass(const GLuint exposured_color_buffer,
                                 const GLuint fullscreen_mesh_vao,
                                 const GLuint width, const GLuint height)
    : exposured_color_buffer_(exposured_color_buffer),
      fullscreen_mesh_vao_(fullscreen_mesh_vao),
      shader_program_(CreateProgram("shader/TonemappingPass.vert",
                                    "shader/TonemappingPass.frag")) {}

TonemappingPass::~TonemappingPass() { glDeleteProgram(shader_program_); }

}  // namespace game