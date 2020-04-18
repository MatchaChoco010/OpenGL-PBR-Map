#include "specular_ibl_pass.h"

namespace game {

void SpecularIblPass::Render(const Scene& scene) const {
  // Global Specular IBL
  glUseProgram(shader_program_);
  glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_);

  glDisable(GL_DEPTH_TEST);

  const glm::vec3 world_camera_pos = scene.camera_->GetPosition();
  const auto view_projection_i =
      glm::inverse(scene.camera_->GetViewProjectionMatrix());
  const auto projection_params =
      glm::vec2(scene.camera_->GetNear(), scene.camera_->GetFar());

  glUniform3fv(world_camera_pos_loc_, 1, &world_camera_pos[0]);
  glUniformMatrix4fv(view_projection_i_loc_, 1, GL_FALSE,
                     &view_projection_i[0][0]);
  glUniform2fv(projection_params_loc_, 1, &projection_params[0]);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gbuffer0_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gbuffer1_);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gbuffer2_);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP,
                scene.global_specular_ibl_texture_->GetTextureId());
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, scene.specular_ibl_lut_texture_->GetTextureId());

  glBindVertexArray(fullscreen_vao_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

SpecularIblPass::SpecularIblPass(const GLuint hdr_fbo, const GLuint gbuffer0,
                                 const GLuint gbuffer1, const GLuint gbuffer2,
                                 const GLuint fullscreen_vao)
    : hdr_fbo_(hdr_fbo),
      gbuffer0_(gbuffer0),
      gbuffer1_(gbuffer1),
      gbuffer2_(gbuffer2),
      fullscreen_vao_(fullscreen_vao),
      shader_program_(CreateProgram("shader/SpecularIBLPass.vert",
                                    "shader/SpecularIBLPass.frag")),
      world_camera_pos_loc_(
          glGetUniformLocation(shader_program_, "worldCameraPos")),
      view_projection_i_loc_(
          glGetUniformLocation(shader_program_, "ViewProjectionI")),
      projection_params_loc_(
          glGetUniformLocation(shader_program_, "ProjectionParams")) {}

SpecularIblPass::~SpecularIblPass() { glDeleteProgram(shader_program_); }

}  // namespace game