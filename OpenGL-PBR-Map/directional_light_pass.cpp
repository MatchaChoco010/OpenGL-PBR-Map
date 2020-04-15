#include "directional_light_pass.h"

namespace game {

void DirectionalLightPass::Render(const Scene& scene) const {
  // Shadow Pass
  glUseProgram(shadow_pass_shader_program_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo_);

  glPolygonOffset(2.0f, 5.0f);
  glEnable(GL_POLYGON_OFFSET_FILL);

  glStencilFunc(GL_ALWAYS, 255, 255);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  glViewport(0, 0, shadow_map_size_, shadow_map_size_);

  glClear(GL_DEPTH_BUFFER_BIT);

  auto light_view_projection_matrix =
      scene.directional_light_->GetLightViewProjectionMatrix();

  for (const auto& mesh_entity : scene.mesh_entities_) {
    auto model_view_projection =
        light_view_projection_matrix * mesh_entity.GetModelMatrix();
    glUniformMatrix4fv(shadow_pass_model_view_projection_loc_, 1, GL_FALSE,
                       &model_view_projection[0][0]);
    mesh_entity.mesh_->Draw();
  }

  // Lighting Pass
  glUseProgram(shader_program_);
  glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_);

  glViewport(0, 0, width_, height_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gbuffer0_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gbuffer1_);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gbuffer2_);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, shadow_map_);

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
  glUniformMatrix4fv(light_view_projection_loc_, 1, GL_FALSE,
                     &light_view_projection_matrix[0][0]);

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

      shadow_map_size_(4096),

      hdr_fbo_(hdr_fbo),
      gbuffer0_(gbuffer0),
      gbuffer1_(gbuffer1),
      gbuffer2_(gbuffer2),
      fullscreen_vao_(fullscreen_vao),

      shadow_map_(CreateShadowMap(shadow_map_size_)),
      shadow_map_fbo_(CreateShadowMapFbo(shadow_map_)),

      shadow_pass_shader_program_(
          CreateProgram("shader/DirectionalLightShadowPass.vert",
                        "shader/DirectionalLightShadowPass.frag")),
      shadow_pass_model_view_projection_loc_(glGetUniformLocation(
          shadow_pass_shader_program_, "ModelViewProjection")),

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
          glGetUniformLocation(shader_program_, "ProjectionParams")),
      light_view_projection_loc_(
          glGetUniformLocation(shader_program_, "LightViewProjection")) {}

DirectionalLightPass::~DirectionalLightPass() {
  glDeleteFramebuffers(1, &shadow_map_fbo_);
  glDeleteRenderbuffers(1, &shadow_map_);

  glDeleteProgram(shadow_pass_shader_program_);
  glDeleteProgram(shader_program_);
}

const GLuint DirectionalLightPass::CreateShadowMap(
    const GLuint shadow_map_size) {
  GLuint shadow_map;
  glGenTextures(1, &shadow_map);
  glBindTexture(GL_TEXTURE_2D, shadow_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadow_map_size,
               shadow_map_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                  GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  return shadow_map;
}

const GLuint DirectionalLightPass::CreateShadowMapFbo(const GLuint shadow_map) {
  GLuint shadow_map_fbo;
  glGenFramebuffers(1, &shadow_map_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadow_map, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return shadow_map_fbo;
}

}  // namespace game