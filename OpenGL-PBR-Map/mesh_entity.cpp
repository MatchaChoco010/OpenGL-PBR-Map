#include "mesh_entity.h"

namespace game {

const glm::vec3 MeshEntity::GetPosition() const { return position_; }

void MeshEntity::SetPosition(const glm::vec3 position) {
  position_ = position;
  RecaluculateModelMatrix();
}

const glm::vec3 MeshEntity::GetRotation() const { return rotation_; }

void MeshEntity::SetRotation(const glm::vec3 rotation) {
  rotation_ = rotation;
  RecaluculateModelMatrix();
}

const glm::vec3 MeshEntity::GetScale() const { return scale_; }

void MeshEntity::SetScale(const glm::vec3 scale) {
  scale_ = scale;
  RecaluculateModelMatrix();
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
}

void MeshEntity::RecaluculateModelMatrix() {
  auto scale = glm::scale(glm::mat4(1), scale_);
  auto rotation = glm::eulerAngleYXZ(rotation_.y, rotation_.x, rotation_.z);
  auto translate = glm::translate(glm::mat4(1), position_);
  model_matrix_ = translate * rotation * scale;
}

}  // namespace game