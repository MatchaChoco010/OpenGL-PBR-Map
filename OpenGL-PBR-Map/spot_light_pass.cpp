#include "spot_light_pass.h"

namespace game {

void SpotLightPass::Render(const Scene& scene) const {
  for (const SpotLight& spot_light : scene.spot_lights_) {
    // Shadow Map Pass
    glUseProgram(shadow_pass_shader_program_);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo_);

    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glPolygonOffset(8.0f, 1.0f);
    glEnable(GL_POLYGON_OFFSET_FILL);

    glViewport(0, 0, shadow_map_size_, shadow_map_size_);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT);

    const glm::mat4 light_view_projection =
        spot_light.GetLightViewProjectionMatrix();

    for (const auto& mesh_entity : scene.mesh_entities_) {
      if (!mesh_entity.TestSphereAABB(spot_light.GetPosition(),
                                     spot_light.GetRange())) {
        continue;
      }

      const glm::mat4 model_view_projection =
          light_view_projection * mesh_entity.GetModelMatrix();

      glUniformMatrix4fv(shadow_pass_model_view_projection_loc_, 1, GL_FALSE,
                         &model_view_projection[0][0]);
      mesh_entity.mesh_->Draw();
    }

    // Stencil Pass
    glUseProgram(stencil_pass_shader_program_);
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_color_fbo_);

    glViewport(0, 0, width_, height_);

    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);

    glStencilMask(255);
    glClear(GL_STENCIL_BUFFER_BIT);

    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    glDrawBuffer(GL_NONE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    glm::mat4 model_view_projection =
        scene.camera_->GetViewProjectionMatrix() * spot_light.GetModelMatrix();
    glUniformMatrix4fv(stencil_pass_model_view_projection_loc_, 1, GL_FALSE,
                       &model_view_projection[0][0]);

    glBindVertexArray(sphere_vao_);
    glDrawElements(GL_TRIANGLES, 8 * 8 * 6, GL_UNSIGNED_INT, 0);

    // Lighting Pass
    glUseProgram(shader_program_);

    glDisable(GL_DEPTH_TEST);

    glStencilFunc(GL_NOTEQUAL, 0, 255);
    glStencilMask(0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);

    const glm::vec3 world_light_pos = spot_light.GetPosition();
    const GLfloat light_intensity = spot_light.GetIntensity();
    const glm::vec3 light_color = spot_light.GetColor();
    const GLfloat light_range = spot_light.GetRange();
    const glm::vec3 light_direction = spot_light.GetDirection();
    const GLfloat light_angle = spot_light.GetAngle();
    const GLfloat light_blend = spot_light.GetBlend();
    const glm::vec3 world_camera_pos = scene.camera_->GetPosition();
    const glm::mat4 view_projection_i =
        glm::inverse(scene.camera_->GetViewProjectionMatrix());
    const glm::vec2 projection_params =
        glm::vec2(scene.camera_->GetNear(), scene.camera_->GetFar());

    glUniformMatrix4fv(model_view_projection_loc_, 1, GL_FALSE,
                       &model_view_projection[0][0]);
    glUniform3fv(world_light_position_loc_, 1, &world_light_pos[0]);
    glUniform1fv(light_intensity_loc_, 1, &light_intensity);
    glUniform3fv(light_color_loc_, 1, &light_color[0]);
    glUniform1fv(light_range_loc_, 1, &light_range);
    glUniform3fv(light_direction_loc_, 1, &light_direction[0]);
    glUniform1fv(light_angle_loc_, 1, &light_angle);
    glUniform1fv(light_blend_loc_, 1, &light_blend);
    glUniform3fv(world_camera_pos_loc_, 1, &world_camera_pos[0]);
    glUniformMatrix4fv(view_projection_i_loc_, 1, GL_FALSE,
                       &view_projection_i[0][0]);
    glUniform2fv(projection_params_loc_, 1, &projection_params[0]);
    glUniformMatrix4fv(light_view_projection_loc_, 1, GL_FALSE,
                       &light_view_projection[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer0_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer1_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuffer2_);
    glActiveTexture(GL_TEXTURE3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, shadow_map_);

    glBindVertexArray(sphere_vao_);
    glDrawElements(GL_TRIANGLES, 8 * 8 * 6, GL_UNSIGNED_INT, 0);

    glCullFace(GL_BACK);
  }
}

SpotLightPass::SpotLightPass(const GLuint hdr_color_fbo, const GLuint gbuffer0,
                             const GLuint gbuffer1, const GLuint gbuffer2,
                             const GLuint sphere_vao, const GLuint width,
                             const GLuint height)
    : width_(width),
      height_(height),

      shadow_map_size_(512),

      hdr_color_fbo_(hdr_color_fbo),
      gbuffer0_(gbuffer0),
      gbuffer1_(gbuffer1),
      gbuffer2_(gbuffer2),
      sphere_vao_(sphere_vao),

      shadow_map_(CreateShadowMap(shadow_map_size_)),
      shadow_map_fbo_(CreateShadowMapFbo(shadow_map_)),

      shadow_pass_shader_program_(
          CreateProgram("shader/SpotLightShadowPass.vert",
                        "shader/SpotLightShadowPass.frag")),
      shadow_pass_model_view_projection_loc_(glGetUniformLocation(
          shadow_pass_shader_program_, "ModelViewProjection")),

      stencil_pass_shader_program_(
          CreateProgram("shader/PunctualLightStencilPass.vert",
                        "shader/PunctualLightStencilPass.frag")),
      stencil_pass_model_view_projection_loc_(glGetUniformLocation(
          stencil_pass_shader_program_, "ModelViewProjection")),

      shader_program_(CreateProgram("shader/SpotLightPass.vert",
                                    "shader/SpotLightPass.frag")),
      model_view_projection_loc_(
          glGetUniformLocation(shader_program_, "ModelViewProjection")),
      world_light_position_loc_(
          glGetUniformLocation(shader_program_, "worldLightPosition")),
      light_intensity_loc_(
          glGetUniformLocation(shader_program_, "lightIntensity")),
      light_color_loc_(glGetUniformLocation(shader_program_, "lightColor")),
      light_range_loc_(glGetUniformLocation(shader_program_, "lightRange")),
      light_direction_loc_(
          glGetUniformLocation(shader_program_, "lightDirection")),
      light_angle_loc_(glGetUniformLocation(shader_program_, "lightAngle")),
      light_blend_loc_(glGetUniformLocation(shader_program_, "lightBlend")),
      world_camera_pos_loc_(
          glGetUniformLocation(shader_program_, "worldCameraPos")),
      view_projection_i_loc_(
          glGetUniformLocation(shader_program_, "ViewProjectionI")),
      projection_params_loc_(
          glGetUniformLocation(shader_program_, "ProjectionParams")),
      light_view_projection_loc_(
          glGetUniformLocation(shader_program_, "LightViewProjection")) {}

SpotLightPass::~SpotLightPass() {
  glDeleteFramebuffers(1, &shadow_map_fbo_);
  glDeleteTextures(1, &shadow_map_);

  glDeleteProgram(shadow_pass_shader_program_);
  glDeleteProgram(stencil_pass_shader_program_);
  glDeleteProgram(shader_program_);
}

const GLuint SpotLightPass::CreateShadowMap(const GLuint shadow_map_size) {
  GLuint shadow_map;
  glGenTextures(1, &shadow_map);
  glBindTexture(GL_TEXTURE_2D, shadow_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_size,
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

const GLuint SpotLightPass::CreateShadowMapFbo(const GLuint shadow_map) {
  GLuint shadow_map_fbo;
  glGenFramebuffers(1, &shadow_map_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadow_map, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return shadow_map_fbo;
}

}  // namespace game