#include "mesh.h"

namespace game {

void Mesh::Draw() const {
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, size_);
}

Mesh::Mesh(const std::vector<glm::vec3>& vertices,
           const std::vector<glm::vec3>& colors)
    : size_(vertices.size()) {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vertices_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
               &vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  glGenBuffers(1, &colors_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo_);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0],
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Mesh::~Mesh() { Release(); }

Mesh::Mesh(Mesh&& other) noexcept
    : size_(std::move(other.size_)),
      vao_(other.vao_),
      vertices_vbo_(other.vertices_vbo_),
      colors_vbo_(other.colors_vbo_) {
  other.vao_ = 0;

  other.vertices_vbo_ = 0;
  other.colors_vbo_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
  if (this != &other) {
    Release();

    size_ = std::move(other.size_);

    vao_ = other.vao_;
    other.vao_ = 0;

    vertices_vbo_ = other.vertices_vbo_;
    colors_vbo_ = other.colors_vbo_;
    other.vertices_vbo_ = 0;
    other.colors_vbo_ = 0;
  }

  return *this;
}

std::unique_ptr<Mesh> Mesh::CreateTriangleMesh() {
  // 頂点位置
  const std::vector<glm::vec3> vertices = {
      glm::vec3(0.0f, 0.5f, 0.0f),
      glm::vec3(-0.5f, -0.5f, 0.0f),
      glm::vec3(0.5f, -0.5f, 0.0f),
  };
  // 頂点カラー
  const std::vector<glm::vec3> colors = {
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),
  };
  return std::make_unique<Mesh>(vertices, colors);
}

void Mesh::Release() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vertices_vbo_);
  glDeleteBuffers(1, &colors_vbo_);
}

}  // namespace game