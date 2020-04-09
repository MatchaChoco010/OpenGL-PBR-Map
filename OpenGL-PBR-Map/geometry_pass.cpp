#include "geometry_pass.h"

namespace game {

void GeometryPass::Render(const Scene& scene) {
  glEnable(GL_STENCIL_TEST);

  glStencilFunc(GL_ALWAYS, 128, 128);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilMask(255);
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo_);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0);
  glClearStencil(0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glUseProgram(shader_program_);

  const GLenum bufs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                         GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, bufs);

  const auto view = scene.camera_->GetViewMatrix();
  const auto projection = scene.camera_->GetProjectionMatrix();
  const auto projection_params =
      glm::vec2(scene.camera_->GetNear(), scene.camera_->GetFar());

  glUniformMatrix4fv(view_loc_, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projection_loc_, 1, GL_FALSE, &projection[0][0]);
  glUniform2fv(projection_params_loc_, 1, &projection_params[0]);

  for (const auto& mesh_entity : scene.mesh_entities_) {
    const auto model = mesh_entity.GetModelMatrix();
    const auto model_it = glm::inverseTranspose(model);

    glUniformMatrix4fv(model_loc_, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(model_it_loc_, 1, GL_FALSE, &model_it[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  mesh_entity.material_->albedo_map_.GetTextureId());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,
                  mesh_entity.material_->metallic_map_.GetTextureId());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,
                  mesh_entity.material_->roughness_map_.GetTextureId());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,
                  mesh_entity.material_->normal_map_.GetTextureId());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,
                  mesh_entity.material_->emissive_map_.GetTextureId());

    glUniform1fv(emissive_intensity_loc_, 1,
                 &mesh_entity.material_->emissive_intensity_);

    mesh_entity.mesh_->Draw();
  }
}

GeometryPass::GeometryPass(const GLuint gbuffer_fbo)
    : gbuffer_fbo_(gbuffer_fbo),
      shader_program_(CreateProgram("shader/GeometryPass.vert",
                                    "shader/GeometryPass.frag")),
      model_loc_(glGetUniformLocation(shader_program_, "Model")),
      model_it_loc_(glGetUniformLocation(shader_program_, "ModelIT")),
      view_loc_(glGetUniformLocation(shader_program_, "View")),
      projection_loc_(glGetUniformLocation(shader_program_, "Projection")),
      projection_params_loc_(
          glGetUniformLocation(shader_program_, "ProjectionParams")),
      emissive_intensity_loc_(
          glGetUniformLocation(shader_program_, "emissiveIntensity")) {}

GeometryPass::~GeometryPass() { glDeleteProgram(shader_program_); }

}  // namespace game