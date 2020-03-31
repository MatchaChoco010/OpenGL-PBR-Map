#include "mesh.h"

namespace game {

void Mesh::Draw() const {
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, size_);
}

Mesh::Mesh(const std::vector<glm::vec3>& vertices,
           const std::vector<glm::vec3>& normals)
    : size_(vertices.size()) {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vertices_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
               &vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  glGenBuffers(1, &normals_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo_);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
               &normals[0], GL_STATIC_DRAW);
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
      normals_vbo_(other.normals_vbo_) {
  other.vao_ = 0;

  other.vertices_vbo_ = 0;
  other.normals_vbo_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
  if (this != &other) {
    Release();

    size_ = std::move(other.size_);

    vao_ = other.vao_;
    other.vao_ = 0;

    vertices_vbo_ = other.vertices_vbo_;
    normals_vbo_ = other.normals_vbo_;
    other.vertices_vbo_ = 0;
    other.normals_vbo_ = 0;
  }

  return *this;
}

std::vector<std::string> Mesh::SplitString(const std::string& s, char delim) {
  std::vector<std::string> elems(0);
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::shared_ptr<Mesh> Mesh::LoadObjMesh(const std::string file) {
  std::vector<unsigned int> vertex_indices, normal_indices;
  std::vector<glm::vec3> tmp_vertices;
  std::vector<glm::vec3> tmp_normals;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;

  std::ifstream ifs(file);
  std::string line;
  if (ifs.fail()) {
    std::cerr << "Can't open obj file: " << file << std::endl;
    return nullptr;
  }
  while (getline(ifs, line)) {
    auto col = SplitString(line, ' ');

    if (col[0] == "v") {
      tmp_vertices.emplace_back(std::stof(col[1]), std::stof(col[2]),
                                std::stof(col[3]));
    } else if (col[0] == "vn") {
      tmp_normals.emplace_back(std::stof(col[1]), std::stof(col[2]),
                              std::stof(col[3]));
    } else if (col[0] == "f") {
      auto v1 = SplitString(col[1], '/');
      auto v2 = SplitString(col[2], '/');
      auto v3 = SplitString(col[3], '/');
      vertex_indices.emplace_back(std::stoi(v1[0]));
      vertex_indices.emplace_back(std::stoi(v2[0]));
      vertex_indices.emplace_back(std::stoi(v3[0]));
      normal_indices.emplace_back(std::stoi(v1[2]));
      normal_indices.emplace_back(std::stoi(v2[2]));
      normal_indices.emplace_back(std::stoi(v3[2]));
    }
  }

  for (unsigned int i = 0; i < vertex_indices.size(); i++) {
    unsigned int vertex_index = vertex_indices[i];
    vertices.emplace_back(tmp_vertices[vertex_index - 1]);
  }
  for (unsigned int i = 0; i < normal_indices.size(); i++) {
    unsigned int normal_index = normal_indices[i];
    normals.emplace_back(tmp_normals[normal_index - 1]);
  }

  return std::make_shared<Mesh>(vertices, normals);
}

void Mesh::Release() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vertices_vbo_);
  glDeleteBuffers(1, &normals_vbo_);
}

}  // namespace game