#include "sky_pass.h"

namespace game {

void SkyPass::Render(const Scene& scene) const {
  glUseProgram(shader_program_);
  glBindFramebuffer(GL_FRAMEBUFFER, hdr_color_fbo_);

  glClear(GL_COLOR_BUFFER_BIT);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  glStencilFunc(GL_ALWAYS, 0, 0);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  glDisable(GL_BLEND);

  const GLfloat y_tan = std::tan(scene.camera_->GetFovY() / 2.0f);
  const GLfloat x_tan =
      std::tan(scene.camera_->GetFovY() * scene.camera_->GetAspect() / 2.0f);
  const glm::mat4 camera_rotation_matrix = scene.camera_->GetRotationMatrix();

  glUniform1fv(y_tan_loc_, 1, &y_tan);
  glUniform1fv(x_tan_loc_, 1, &x_tan);
  glUniformMatrix4fv(camera_rotation_matrix_loc_, 1, GL_FALSE,
                     &camera_rotation_matrix[0][0]);
  glUniform1fv(sky_intensity_loc_, 1, &scene.sky_intensity_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, scene.sky_texture_->GetTextureId());

  glBindVertexArray(fullscreen_mesh_vao_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

SkyPass::SkyPass(const GLuint hdr_color_fbo, const GLuint fullscreen_mesh_vao,
                 const GLuint width, const GLuint height)
    : width_(width),
      height_(height),

      hdr_color_fbo_(hdr_color_fbo),

      fullscreen_mesh_vao_(fullscreen_mesh_vao),

      shader_program_(
          CreateProgram("shader/SkyPass.vert", "shader/SkyPass.frag")),
      y_tan_loc_(glGetUniformLocation(shader_program_, "yTan")),
      x_tan_loc_(glGetUniformLocation(shader_program_, "xTan")),
      camera_rotation_matrix_loc_(
          glGetUniformLocation(shader_program_, "CameraRotationMatrix")),
      sky_intensity_loc_(
          glGetUniformLocation(shader_program_, "skyIntensity")) {}

SkyPass::~SkyPass() { glDeleteProgram(shader_program_); }

}  // namespace game