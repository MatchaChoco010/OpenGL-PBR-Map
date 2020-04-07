#include "spot_light.h"

namespace game {

const glm::vec3 SpotLight::GetPosition() const { return position_; }

void SpotLight::SetPosition(const glm::vec3 position) {
  position_ = position;
  RecaluculateModelMatrix();
}

const GLfloat SpotLight::GetIntensity() const { return intensity_; }

void SpotLight::SetIntensity(const GLfloat intensity) {
  intensity_ = intensity;
}

const glm::vec3 SpotLight::GetColor() const { return color_; }

void SpotLight::SetColor(const glm::vec3 color) { color_ = color; }

const GLfloat SpotLight::GetRange() const { return range_; }

void SpotLight::SetRange(const GLfloat range) {
  range_ = range;
  RecaluculateModelMatrix();
}

const glm::vec3 SpotLight::GetDirection() const { return direction_; }

void SpotLight::SetDirection(const glm::vec3 direction) {
  direction_ = glm::normalize(direction);
}

const GLfloat SpotLight::GetAngle() const { return angle_; }

void SpotLight::SetAngle(const GLfloat angle) { angle_ = angle; }

const GLfloat SpotLight::GetBlend() const { return blend_; }

void SpotLight::SetBlend(const GLfloat blend) { blend_ = blend; }

const glm::mat4 SpotLight::GetModelMatrix() const { return model_matrix_; }

SpotLight::SpotLight(const glm::vec3 position, const GLfloat intensity,
                     const glm::vec3 color, const GLfloat range,
                     const glm::vec3 direction, const GLfloat angle,
                     const GLfloat blend)
    : position_(position),
      intensity_(intensity),
      color_(color),
      range_(range),
      direction_(glm::normalize(direction)),
      angle_(angle),
      blend_(blend) {
  RecaluculateModelMatrix();
}

void SpotLight::RecaluculateModelMatrix() {
  model_matrix_ = glm::translate(glm::mat4(1), position_) *
                  glm::scale(glm::mat4(1), glm::vec3(range_ + 0.1f));
}

}  // namespace game