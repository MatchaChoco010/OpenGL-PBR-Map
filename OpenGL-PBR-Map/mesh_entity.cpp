#include "mesh_entity.h"

namespace game {

const glm::vec3 MeshEntity::GetPosition() const { return position_; }

void MeshEntity::SetPosition(const glm::vec3 position) {
  position_ = position;
  RecaluculateModelMatrix();
  RecaluculateAABB();
}

const glm::vec3 MeshEntity::GetRotation() const { return rotation_; }

void MeshEntity::SetRotation(const glm::vec3 rotation) {
  rotation_ = rotation;
  RecaluculateModelMatrix();
  RecaluculateAABB();
}

const glm::vec3 MeshEntity::GetScale() const { return scale_; }

void MeshEntity::SetScale(const glm::vec3 scale) {
  scale_ = scale;
  RecaluculateModelMatrix();
  RecaluculateAABB();
}

const glm::mat4 MeshEntity::GetModelMatrix() const { return model_matrix_; }

MeshEntity::MeshEntity(const std::shared_ptr<const Mesh> mesh,
                       const std::shared_ptr<const Material> material,
                       const glm::vec3 position, const glm::vec3 rotation,
                       const glm::vec3 scale)
    : mesh_(mesh),
      material_(material),
      position_(position),
      rotation_(rotation),
      scale_(scale),
      model_matrix_() {
  RecaluculateModelMatrix();
  RecaluculateAABB();
}

void MeshEntity::RecaluculateModelMatrix() {
  auto scale = glm::scale(glm::mat4(1), scale_);
  auto rotation = glm::eulerAngleYXZ(rotation_.y, rotation_.x, rotation_.z);
  auto translate = glm::translate(glm::mat4(1), position_);
  model_matrix_ = translate * rotation * scale;
}

void MeshEntity::RecaluculateAABB() {
  std::vector<glm::vec3> vertices = {
      glm::vec3(mesh_->x_min_, mesh_->y_min_, mesh_->z_min_),
      glm::vec3(mesh_->x_min_, mesh_->y_min_, mesh_->z_max_),
      glm::vec3(mesh_->x_min_, mesh_->y_max_, mesh_->z_min_),
      glm::vec3(mesh_->x_min_, mesh_->y_max_, mesh_->z_max_),
      glm::vec3(mesh_->x_max_, mesh_->y_min_, mesh_->z_min_),
      glm::vec3(mesh_->x_max_, mesh_->y_min_, mesh_->z_max_),
      glm::vec3(mesh_->x_max_, mesh_->y_max_, mesh_->z_min_),
      glm::vec3(mesh_->x_max_, mesh_->y_max_, mesh_->z_max_),
  };
  x_max_ = std::numeric_limits<float>::min();
  y_max_ = std::numeric_limits<float>::min();
  z_max_ = std::numeric_limits<float>::min();
  x_min_ = std::numeric_limits<float>::max();
  y_min_ = std::numeric_limits<float>::max();
  z_min_ = std::numeric_limits<float>::max();
  for (const auto& vertex : vertices) {
    auto vec = model_matrix_ * glm::vec4(vertex, 1.0f);
    auto v = glm::vec3(vec.x, vec.y, vec.z) / vec.w;
    if (v.x > x_max_) x_max_ = v.x;
    if (v.y > y_max_) y_max_ = v.y;
    if (v.z > z_max_) z_max_ = v.z;
    if (v.x < x_min_) x_min_ = v.x;
    if (v.y < y_min_) y_min_ = v.y;
    if (v.z < z_min_) z_min_ = v.z;
  }
}

}  // namespace game