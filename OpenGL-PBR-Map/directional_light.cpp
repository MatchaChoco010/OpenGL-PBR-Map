#include "directional_light.h"

namespace game {
const GLfloat DirectionalLight::GetIntensity() const { return intensity_; }

void DirectionalLight::SetIntensity(const GLfloat intensity) {
  intensity_ = intensity;
}

const glm::vec3 DirectionalLight::GetDirection() const { return direction_; }

void DirectionalLight::SetDirection(const glm::vec3 direction) {
  direction_ = glm::normalize(direction);
}

const glm::vec3 DirectionalLight::GetColor() const { return color_; }

void DirectionalLight::SetColor(const glm::vec3 color) { color_ = color; }

void DirectionalLight::SetShadowVolume(const GLfloat left, const GLfloat right,
                                       const GLfloat bottom, const GLfloat top,
                                       const GLfloat near, const GLfloat far,
                                       const glm::mat4 light_view) {
  auto light_projection = glm::ortho(left, right, bottom, top, near, far);
  light_view_projection_matrix_ = light_projection * light_view;
}

const glm::mat4 DirectionalLight::GetLightViewProjectionMatrix() const {
  return light_view_projection_matrix_;
}

DirectionalLight::DirectionalLight(const GLfloat intensity,
                                   const glm::vec3 direction,
                                   const glm::vec3 color)
    : intensity_(intensity),
      direction_(glm::normalize(direction)),
      color_(color) {
  SetShadowVolume(-10.0, 10.0, -10.0f, 10.0f, 0, 50.0f, glm::mat4());
}

}  // namespace game