#include "point_light_pass.h"

namespace game {

void PointLightPass::Render(const Scene& scene) const {
  for (const PointLight& point_light : scene.point_lights_) {
    // Shadow Map Pass
    if (point_light.GetUseShadow()) {
      glUseProgram(shadow_pass_shader_program_);
      glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo_);

      glStencilFunc(GL_ALWAYS, 0, 0);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

      glViewport(0, 0, shadow_map_size_, shadow_map_size_);

      glDepthMask(GL_TRUE);
      glEnable(GL_DEPTH_TEST);

      glClear(GL_DEPTH_BUFFER_BIT);

      glm::mat4 light_view_projection_matrices[6];
      point_light.GetLightViewProjectionMatrices(
          light_view_projection_matrices);
      const glm::vec3 position = point_light.GetPosition();
      const GLfloat range = point_light.GetRange();
      glUniformMatrix4fv(shadow_pass_light_view_projection_matrices_loc_, 6,
                         GL_FALSE, &light_view_projection_matrices[0][0][0]);
      glUniform3fv(shadow_pass_world_light_pos_loc_, 1, &position[0]);
      glUniform1fv(shadow_pass_range_loc_, 1, &range);

      for (const auto& mesh_entity : scene.mesh_entities_) {
        if (!mesh_entity.TestSphereAABB(position, range)) {
          continue;
        }

        const glm::mat4 model = mesh_entity.GetModelMatrix();

        glUniformMatrix4fv(shadow_pass_model_loc_, 1, GL_FALSE, &model[0][0]);
        mesh_entity.mesh_->Draw();
      }
    }

    // Stencil Pass
    glUseProgram(stencil_pass_shader_program_);
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_);

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
        scene.camera_->GetViewProjectionMatrix() * point_light.GetModelMatrix();
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

    const glm::vec3 world_light_pos = point_light.GetPosition();
    const GLfloat light_intensity = point_light.GetIntensity();
    const glm::vec3 light_color = point_light.GetColor();
    const GLfloat light_range = point_light.GetRange();
    const glm::vec3 world_camera_pos = scene.camera_->GetPosition();
    const glm::mat4 view_projection_i =
        glm::inverse(scene.camera_->GetViewProjectionMatrix());
    const glm::vec2 projection_params =
        glm::vec2(scene.camera_->GetNear(), scene.camera_->GetFar());
    const GLfloat shadow_bias = point_light.GetShadowBias();
    const GLint use_shadow = point_light.GetUseShadow() ? 1 : 0;

    glUniformMatrix4fv(model_view_projection_loc_, 1, GL_FALSE,
                       &model_view_projection[0][0]);
    glUniform3fv(world_light_position_loc_, 1, &world_light_pos[0]);
    glUniform1fv(light_intensity_loc_, 1, &light_intensity);
    glUniform3fv(light_color_loc_, 1, &light_color[0]);
    glUniform1fv(light_range_loc_, 1, &light_range);
    glUniform3fv(world_camera_pos_loc_, 1, &world_camera_pos[0]);
    glUniformMatrix4fv(view_projection_i_loc_, 1, GL_FALSE,
                       &view_projection_i[0][0]);
    glUniform2fv(projection_params_loc_, 1, &projection_params[0]);
    glUniform1fv(shadow_bias_loc_, 1, &shadow_bias);
    glUniform1i(use_shadow_loc_, use_shadow);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer0_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer1_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuffer2_);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map_);

    glBindVertexArray(sphere_vao_);
    glDrawElements(GL_TRIANGLES, 8 * 8 * 6, GL_UNSIGNED_INT, 0);

    glCullFace(GL_BACK);
  }
}

PointLightPass::PointLightPass(const GLuint hdr_fbo, const GLuint gbuffer0,
                               const GLuint gbuffer1, const GLuint gbuffer2,
                               const GLuint sphere_vao, const GLuint width,
                               const GLuint height)
    : width_(width),
      height_(height),

      shadow_map_size_(512),

      hdr_fbo_(hdr_fbo),
      gbuffer0_(gbuffer0),
      gbuffer1_(gbuffer1),
      gbuffer2_(gbuffer2),
      sphere_vao_(sphere_vao),

      shadow_map_(CreateShadowMap(shadow_map_size_)),
      shadow_map_fbo_(CreateShadowMapFbo(shadow_map_)),

      shadow_pass_shader_program_(
          CreateProgram("shader/PointLightShadowPass.vert",
                        "shader/PointLightShadowPass.geom",
                        "shader/PointLightShadowPass.frag")),
      shadow_pass_model_loc_(
          glGetUniformLocation(shadow_pass_shader_program_, "Model")),
      shadow_pass_light_view_projection_matrices_loc_(
          glGetUniformLocation(shadow_pass_shader_program_, "ShadowMatrices")),
      shadow_pass_world_light_pos_loc_(
          glGetUniformLocation(shadow_pass_shader_program_, "worldLightPos")),
      shadow_pass_range_loc_(
          glGetUniformLocation(shadow_pass_shader_program_, "range")),

      stencil_pass_shader_program_(
          CreateProgram("shader/PunctualLightStencilPass.vert",
                        "shader/PunctualLightStencilPass.frag")),
      stencil_pass_model_view_projection_loc_(glGetUniformLocation(
          stencil_pass_shader_program_, "ModelViewProjection")),

      shader_program_(CreateProgram("shader/PointLightPass.vert",
                                    "shader/PointLightPass.frag")),
      model_view_projection_loc_(
          glGetUniformLocation(shader_program_, "ModelViewProjection")),
      world_light_position_loc_(
          glGetUniformLocation(shader_program_, "worldLightPosition")),
      light_intensity_loc_(
          glGetUniformLocation(shader_program_, "lightIntensity")),
      light_color_loc_(glGetUniformLocation(shader_program_, "lightColor")),
      light_range_loc_(glGetUniformLocation(shader_program_, "lightRange")),
      world_camera_pos_loc_(
          glGetUniformLocation(shader_program_, "worldCameraPos")),
      view_projection_i_loc_(
          glGetUniformLocation(shader_program_, "ViewProjectionI")),
      projection_params_loc_(
          glGetUniformLocation(shader_program_, "ProjectionParams")),
      shadow_bias_loc_(glGetUniformLocation(shader_program_, "shadowBias")),
      use_shadow_loc_(glGetUniformLocation(shader_program_, "useShadow")) {}

PointLightPass::~PointLightPass() {
  glDeleteFramebuffers(1, &shadow_map_fbo_);
  glDeleteTextures(1, &shadow_map_);

  glDeleteProgram(shadow_pass_shader_program_);
  glDeleteProgram(stencil_pass_shader_program_);
  glDeleteProgram(shader_program_);
}

const GLuint PointLightPass::CreateShadowMap(const GLuint shadow_map_size) {
  GLuint shadow_map;
  glGenTextures(1, &shadow_map);
  glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE,
                  GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  for (int i = 0; i < 6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 shadow_map_size, shadow_map_size, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);
  }
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return shadow_map;
}

const GLuint PointLightPass::CreateShadowMapFbo(const GLuint shadow_map) {
  GLuint shadow_map_fbo;
  glGenFramebuffers(1, &shadow_map_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return shadow_map_fbo;
}

}  // namespace game