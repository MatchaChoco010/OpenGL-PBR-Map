#include "camera.h"

namespace game {

const glm::vec3 Camera::GetPosition() const { return position_; }

void Camera::SetPosition(const glm::vec3 position) {
  position_ = position;
  RecaluculateViewMatirx();
}

const glm::vec3 Camera::GetRotation() const { return rotation_; }

void Camera::SetRotation(const glm::vec3 rotation) {
  rotation_ = rotation;
  RecaluculateViewMatirx();
}

const GLfloat Camera::GetNear() const { return near_; }

const GLfloat Camera::GetFar() const { return far_; }

void Camera::Perspective(const GLfloat fovy, const GLfloat aspect,
                         const GLfloat near, const GLfloat far) {
  near_ = near;
  far_ = far;
  projection_matrix_ = glm::perspective(fovy, aspect, near, far);
}

const glm::mat4 Camera::GetViewProjectionMatrix() const {
  return projection_matrix_ * view_matrix_;
}

const glm::mat4 Camera::GetViewMatrix() const { return view_matrix_; }

const glm::mat4 Camera::GetProjectionMatrix() const {
  return projection_matrix_;
}

Camera::Camera(const glm::vec3 position, const glm::vec3 rotation,
               const GLfloat fovy, const GLfloat aspect, const GLfloat near,
               const GLfloat far)
    : near_(near),
      far_(far),
      position_(position),
      rotation_(rotation),
      view_matrix_(),
      projection_matrix_(glm::perspective(fovy, aspect, near, far)) {
  RecaluculateViewMatirx();
}

void Camera::RecaluculateViewMatirx() {
  auto rotation = glm::eulerAngleYXZ(rotation_.y, rotation_.x, rotation_.z);
  auto translate = glm::translate(glm::mat4(1), position_);
  view_matrix_ = glm::inverse(translate * rotation);
}

}  // namespace game