#include "exposure_pass.h"

namespace game {

void ExposurePass::Render() const {
  glDisable(GL_BLEND);

  glStencilFunc(GL_ALWAYS, 255, 255);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilMask(0);

  glUseProgram(shader_program_);
  glBindFramebuffer(GL_FRAMEBUFFER, exposured_fbo_);
  glViewport(0, 0, width_, height_);

  glUniform1fv(exposure_loc_, 1, &exposure_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdr_color_buffer_);

  glBindVertexArray(fullscreen_mesh_vao_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ExposurePass::SetExposure(const GLfloat exposure) { exposure_ = exposure; }

ExposurePass::ExposurePass(const GLuint hdr_color_buffer,
                           const GLuint exposured_fbo,
                           const GLuint fullscreen_mesh_vao, const GLuint width,
                           const GLuint height)
    : width_(width),
      height_(height),
      hdr_color_buffer_(hdr_color_buffer),
      exposured_fbo_(exposured_fbo),
      fullscreen_mesh_vao_(fullscreen_mesh_vao),
      shader_program_(CreateProgram("shader/ExposurePass.vert",
                                    "shader/ExposurePass.frag")),
      exposure_loc_(glGetUniformLocation(shader_program_, "exposure")),
      exposure_(0) {}

ExposurePass::~ExposurePass() { glDeleteProgram(shader_program_); }

}  // namespace game