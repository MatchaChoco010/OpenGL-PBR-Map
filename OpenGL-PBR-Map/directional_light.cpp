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

DirectionalLight::DirectionalLight(const GLfloat intensity,
                                   const glm::vec3 direction,
                                   const glm::vec3 color)
    : intensity_(intensity),
      direction_(glm::normalize(direction)),
      color_(color) {}

}  // namespace game