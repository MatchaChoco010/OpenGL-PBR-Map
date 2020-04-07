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

const GLfloat PointLight::GetRange() const { return range_; }

void PointLight::SetRange(const GLfloat range) {
  range_ = range;
  RecaluculateModelMatrix();
}

const glm::mat4 PointLight::GetModelMatrix() const { return model_matrix_; }

PointLight::PointLight(const glm::vec3 position, const GLfloat intensity,
                       const glm::vec3 color, const GLfloat range)
    : position_(position),
      intensity_(intensity),
      color_(color),
      range_(range),
      model_matrix_() {
  RecaluculateModelMatrix();
}

void PointLight::RecaluculateModelMatrix() {
  model_matrix_ = glm::translate(glm::mat4(1), position_) *
                  glm::scale(glm::mat4(1), glm::vec3(range_ + 0.1f));
}

}  // namespace game