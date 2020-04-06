#include "directional_light_pass.h"

namespace game {

void DirectionalLightPass::Render(const Scene& scene) const {
  // Lighting Pass
  glUseProgram(shader_program_);
  glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_);

  glClear(GL_COLOR_BUFFER_BIT);

  glStencilFunc(GL_EQUAL, 128, 128);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilMask(0);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  const auto direction = scene.directional_light_->GetDirection();
  const auto intensity = scene.directional_light_->GetIntensity();
  const auto color = scene.directional_light_->GetColor();
  const auto world_camera_pos = scene.camera_->GetPosition();
  const auto view_projection_i =
      glm::inverse(scene.camera_->GetViewProjectionMatrix());
  const auto projection_params =
      glm::vec2(scene.camera_->GetNear(), scene.camera_->GetFar());

  glUniform3fv(light_direction_loc_, 1, &direction[0]);
  glUniform1fv(light_intensity_loc_, 1, &intensity);
  glUniform3fv(light_color_loc_, 1, &color[0]);
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

  glBindVertexArray(fullscreen_vao_);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);
}

DirectionalLightPass::DirectionalLightPass(
    const GLuint hdr_fbo, const GLuint gbuffer0, const GLuint gbuffer1,
    const GLuint gbuffer2, const GLuint fullscreen_vao, const GLuint width,
    const GLuint height)
    : width_(width),
      height_(height),
      hdr_fbo_(hdr_fbo),
      gbuffer0_(gbuffer0),
      gbuffer1_(gbuffer1),
      gbuffer2_(gbuffer2),
      fullscreen_vao_(fullscreen_vao),
      shader_program_(CreateProgram("shader/DirectionalLightPass.vert",
                                    "shader/DirectionalLightPass.frag")),
      light_direction_loc_(
          glGetUniformLocation(shader_program_, "lightDirection")),
      light_intensity_loc_(
          glGetUniformLocation(shader_program_, "lightIntensity")),
      light_color_loc_(glGetUniformLocation(shader_program_, "lightColor")),
      world_camera_pos_loc_(
          glGetUniformLocation(shader_program_, "worldCameraPos")),
      view_projection_i_loc_(
          glGetUniformLocation(shader_program_, "ViewProjectionI")),
      projection_params_loc_(
          glGetUniformLocation(shader_program_, "ProjectionParams")) {}

DirectionalLightPass::~DirectionalLightPass() {
  glDeleteProgram(shader_program_);
}

}  // namespace game