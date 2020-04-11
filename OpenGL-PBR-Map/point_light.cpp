#include "point_light.h"

namespace game {

const glm::vec3 PointLight::GetPosition() const { return position_; }

void PointLight::SetPosition(const glm::vec3 position) {
  position_ = position;
  RecaluculateModelMatrix();
}

const GLfloat PointLight::GetIntensity() const { return intensity_; }

void PointLight::SetIntensity(const GLfloat intensity) {
  intensity_ = intensity;
}

const glm::vec3 PointLight::GetColor() const { return color_; }

void PointLight::SetColor(const glm::vec3 color) { color_ = color; }

const GLfloat PointLight::GetNear() const { return near_; }

void PointLight::SetNear(const GLfloat near) {
  near_ = near;
  RecaluculateLightViewProjectionMatrices();
}

const GLfloat PointLight::GetRange() const { return range_; }

void PointLight::SetRange(const GLfloat range) {
  range_ = range;
  RecaluculateModelMatrix();
  RecaluculateLightViewProjectionMatrices();
}

const GLfloat PointLight::GetShadowBias() const { return shadow_bias_; }

void PointLight::SetShadowBias(const GLfloat shadow_bias) {
  shadow_bias_ = shadow_bias;
}

const bool PointLight::GetUseShadow() const { return use_shadow_; }

void PointLight::SetUseShadow(const bool use_shadow) {
  use_shadow_ = use_shadow;
}

void PointLight::GetLightViewProjectionMatrices(
    glm::mat4 light_view_projection_matrices[6]) const {
  for (int i = 0; i < 6; ++i) {
    light_view_projection_matrices[i] = light_view_projection_matrices_[i];
  }
}

const glm::mat4 PointLight::GetModelMatrix() const { return model_matrix_; }

PointLight::PointLight(const glm::vec3 position, const GLfloat intensity,
                       const glm::vec3 color, const GLfloat near,
                       const GLfloat range, const GLfloat shadow_bias,
                       const bool use_shadow)
    : position_(position),
      intensity_(intensity),
      color_(color),
      near_(near),
      range_(range),
      shadow_bias_(shadow_bias),
      use_shadow_(use_shadow),
      model_matrix_(),
      light_view_projection_matrices_() {
  RecaluculateModelMatrix();
  RecaluculateLightViewProjectionMatrices();
}

void PointLight::RecaluculateLightViewProjectionMatrices() {
  auto light_projection =
      glm::perspective(glm::radians(90.0f), 1.0f, near_, range_);
  light_view_projection_matrices_[0] =
      light_projection * glm::lookAt(position_,
                                     position_ + glm::vec3(1.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, -1.0f, 0.0f));
  light_view_projection_matrices_[1] =
      light_projection * glm::lookAt(position_,
                                     position_ + glm::vec3(-1.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, -1.0f, 0.0f));
  light_view_projection_matrices_[2] =
      light_projection * glm::lookAt(position_,
                                     position_ + glm::vec3(0.0f, 1.0f, 0.0f),
                                     glm::vec3(0.0f, 0.0f, 1.0f));
  light_view_projection_matrices_[3] =
      light_projection * glm::lookAt(position_,
                                     position_ + glm::vec3(0.0f, -1.0f, 0.0f),
                                     glm::vec3(0.0f, 0.0f, -1.0f));
  light_view_projection_matrices_[4] =
      light_projection * glm::lookAt(position_,
                                     position_ + glm::vec3(0.0f, 0.0f, 1.0f),
                                     glm::vec3(0.0f, -1.0f, 0.0f));
  light_view_projection_matrices_[5] =
      light_projection * glm::lookAt(position_,
                                     position_ + glm::vec3(0.0f, 0.0f, -1.0f),
                                     glm::vec3(0.0f, -1.0f, 0.0f));
}

void PointLight::RecaluculateModelMatrix() {
  model_matrix_ = glm::translate(glm::mat4(1), position_) *
                  glm::scale(glm::mat4(1), glm::vec3(range_ + 0.1f));
}

}  // namespace game