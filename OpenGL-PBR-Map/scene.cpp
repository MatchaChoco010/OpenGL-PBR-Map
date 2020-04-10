#include "scene.h"

namespace game {

void Scene::RecaluculateDirectionalShadowVolume() {
  auto x_max = std::numeric_limits<float>::min();
  auto y_max = std::numeric_limits<float>::min();
  auto z_max = std::numeric_limits<float>::min();
  auto x_min = std::numeric_limits<float>::max();
  auto y_min = std::numeric_limits<float>::max();
  auto z_min = std::numeric_limits<float>::max();
  for (const auto& mesh_entity : mesh_entities_) {
    if (x_max < mesh_entity.x_max_) x_max = mesh_entity.x_max_;
    if (y_max < mesh_entity.y_max_) y_max = mesh_entity.y_max_;
    if (z_max < mesh_entity.z_max_) z_max = mesh_entity.z_max_;
    if (x_min > mesh_entity.x_min_) x_min = mesh_entity.x_min_;
    if (y_min > mesh_entity.y_min_) y_min = mesh_entity.y_min_;
    if (z_min > mesh_entity.z_min_) z_min = mesh_entity.z_min_;
  }

  auto dir = glm::normalize(directional_light_->GetDirection());
  auto rot = glm::toMat4(glm::rotation(dir, glm::vec3(0.0f, 0.0f, -1.0f)));

  auto x_max_rotated = std::numeric_limits<float>::min();
  auto y_max_rotated = std::numeric_limits<float>::min();
  auto z_max_rotated = std::numeric_limits<float>::min();
  auto x_min_rotated = std::numeric_limits<float>::max();
  auto y_min_rotated = std::numeric_limits<float>::max();
  auto z_min_rotated = std::numeric_limits<float>::max();
  std::vector<glm::vec4> vertices = {
      glm::vec4(x_min, y_min, z_min, 1.0), glm::vec4(x_min, y_min, z_max, 1.0),
      glm::vec4(x_min, y_max, z_min, 1.0), glm::vec4(x_min, y_max, z_max, 1.0),
      glm::vec4(x_max, y_min, z_min, 1.0), glm::vec4(x_max, y_min, z_max, 1.0),
      glm::vec4(x_max, y_max, z_min, 1.0), glm::vec4(x_max, y_max, z_max, 1.0),
  };
  for (const auto& vertex : vertices) {
    auto v = rot * vertex;
    if (v.x > x_max_rotated) x_max_rotated = v.x;
    if (v.y > y_max_rotated) y_max_rotated = v.y;
    if (v.z > z_max_rotated) z_max_rotated = v.z;
    if (v.x < x_min_rotated) x_min_rotated = v.x;
    if (v.y < y_min_rotated) y_min_rotated = v.y;
    if (v.z < z_min_rotated) z_min_rotated = v.z;
  }

  glm::mat4 translate = glm::translate(
      glm::mat4(1.0f),
      glm::vec3(-(x_max_rotated + x_min_rotated) / 2,
                -(y_max_rotated + y_min_rotated) / 2, -z_max_rotated));

  auto light_view = translate * rot;

  directional_light_->SetShadowVolume(
      -(x_max_rotated - x_min_rotated) / 2, (x_max_rotated - x_min_rotated) / 2,
      -(y_max_rotated - y_min_rotated) / 2, (y_max_rotated - y_min_rotated) / 2,
      0.0f, (z_max_rotated - z_min_rotated), light_view);
}

std::unique_ptr<Scene> Scene::LoadScene(const std::string path,
                                        const GLuint width,
                                        const GLuint height) {
  auto scene = std::make_unique<Scene>();

  auto scenefile_txt_path = path + "/scenefile.txt";
  std::ifstream ifs(scenefile_txt_path);
  std::string line;
  if (ifs.fail()) {
    std::cerr << "Can't open scene file: " << path << std::endl;
    return scene;
  }

  std::unordered_map<std::string, std::shared_ptr<Mesh>> tmp_mesh_map;
  std::unordered_map<std::string, std::shared_ptr<Material>> tmp_material_map;

  while (std::getline(ifs, line)) {
    auto texts = SplitString(line, ' ');

    std::cout << "Load : " << line << std::endl;

    // コメント
    if (texts[0] == "#") continue;

    // Mesh
    else if (texts[0] == "Mesh:") {
      std::string mesh_name = texts[1];

      std::vector<std::string> mesh_texts;
      mesh_texts.emplace_back(line);
      while (std::getline(ifs, line)) {
        mesh_texts.emplace_back(line);
        if (line == "MeshEnd") break;
      }
      auto mesh = ParseMesh(mesh_texts);
      tmp_mesh_map.insert(std::make_pair(mesh_name, mesh));
    }

    // Material
    else if (texts[0] == "Material:") {
      std::string material_name = texts[1];

      std::vector<std::string> material_texts;
      material_texts.emplace_back(line);
      while (std::getline(ifs, line)) {
        material_texts.emplace_back(line);
        if (line == "MaterialEnd") break;
      }
      auto material = ParseMaterial(path, material_texts);
      tmp_material_map.insert(std::make_pair(material_name, material));
    }

    // MeshEntity
    else if (texts[0] == "MeshEntity:") {
      std::shared_ptr<Mesh> mesh;
      std::shared_ptr<Material> material;
      glm::vec3 position;
      glm::vec3 rotation;
      glm::vec3 scale;

      while (std::getline(ifs, line)) {
        auto texts = SplitString(line, ' ');

        if (texts[0] == "Mesh:") {
          mesh = tmp_mesh_map[texts[1]];
        } else if (texts[0] == "Material:") {
          material = tmp_material_map[texts[1]];
        } else if (texts[0] == "Position:") {
          position = glm::vec3(std::stof(texts[1]), std::stof(texts[3]),
                               -std::stof(texts[2]));
        } else if (texts[0] == "Rotation:") {
          rotation = glm::vec3(std::stof(texts[1]), std::stof(texts[3]),
                               -std::stof(texts[2]));
        } else if (texts[0] == "Scale:") {
          scale = glm::vec3(std::stof(texts[1]), std::stof(texts[3]),
                            std::stof(texts[2]));
        }

        if (line == "MeshEntityEnd") break;
      }

      scene->mesh_entities_.emplace_back(mesh, material, position, rotation,
                                         scale);
    }

    // Directional Light
    else if (texts[0] == "DirectionalLight:") {
      GLfloat intensity;
      glm::vec3 direction;
      glm::vec3 color;
      glm::vec3 position;
      GLfloat left = -100;
      GLfloat right = 100;
      GLfloat bottom = -100;
      GLfloat top = 100;
      GLfloat near = -100;
      GLfloat far = 100;

      while (std::getline(ifs, line)) {
        auto texts = SplitString(line, ' ');

        if (texts[0] == "Position:") {
          position = glm::vec3(std::stof(texts[1]), std::stof(texts[3]),
                               -std::stof(texts[2]));
        } else if (texts[0] == "Intensity:") {
          intensity = std::stof(texts[1]);
        } else if (texts[0] == "Color:") {
          color = glm::vec3(std::stof(texts[1]), std::stof(texts[2]),
                            std::stof(texts[3]));
        } else if (texts[0] == "Direction:") {
          direction = glm::vec3(std::stof(texts[1]), std::stof(texts[3]),
                                -std::stof(texts[2]));
        }

        if (line == "DirectionalLightEnd") break;
      }

      scene->directional_light_ =
          std::make_unique<DirectionalLight>(intensity, direction, color);
    }

    // Point Light
    else if (texts[0] == "PointLight:") {
      glm::vec3 position;
      GLfloat intensity;
      glm::vec3 color;
      GLfloat near;
      GLfloat range;
      GLfloat shadow_bias;
      bool use_shadow;

      while (std::getline(ifs, line)) {
        auto texts = SplitString(line, ' ');

        if (texts[0] == "Position:") {
          position = glm::vec3(std::stof(texts[1]), std::stof(texts[3]),
                               -std::stof(texts[2]));
        } else if (texts[0] == "Intensity:") {
          intensity = std::stof(texts[1]);
        } else if (texts[0] == "Color:") {
          color = glm::vec3(std::stof(texts[1]), std::stof(texts[2]),
                            std::stof(texts[3]));
        } else if (texts[0] == "ClipStart:") {
          near = std::stof(texts[1]);
        } else if (texts[0] == "Range:") {
          range = std::stof(texts[1]);
        } else if (texts[0] == "ShadowBias:") {
          shadow_bias = std::stof(texts[1]);
        } else if (texts[0] == "UseShadow:") {
          shadow_bias = std::stoi(texts[1]) == 1;
        }

        if (line == "PointLightEnd") break;
      }

      scene->point_lights_.emplace_back(position, intensity, color, range);
    }

    // Spot Light
    else if (texts[0] == "SpotLight:") {
      glm::vec3 position;
      GLfloat intensity;
      glm::vec3 color;
      GLfloat near;
      GLfloat range;
      glm::vec3 direction;
      GLfloat angle;
      GLfloat blend;

      while (std::getline(ifs, line)) {
        auto texts = SplitString(line, ' ');

        if (texts[0] == "Position:") {
          position = glm::vec3(std::stof(texts[1]), std::stof(texts[3]),
                               -std::stof(texts[2]));
        } else if (texts[0] == "Intensity:") {
          intensity = std::stof(texts[1]);
        } else if (texts[0] == "Color:") {
          color = glm::vec3(std::stof(texts[1]), std::stof(texts[2]),
                            std::stof(texts[3]));
        } else if (texts[0] == "ClipStart:") {
          near = std::stof(texts[1]);
        } else if (texts[0] == "Range:") {
          range = std::stof(texts[1]);
        } else if (texts[0] == "Direction:") {
          direction = glm::vec3(std::stof(texts[1]), std::stof(texts[3]),
                                -std::stof(texts[2]));
        } else if (texts[0] == "Angle:") {
          angle = std::stof(texts[1]);
        } else if (texts[0] == "Blend:") {
          blend = std::stof(texts[1]);
        }

        if (line == "SpotLightEnd") break;
      }

      scene->spot_lights_.emplace_back(position, intensity, color, near, range,
                                       direction, angle, blend);
    }
  }

  scene->RecaluculateDirectionalShadowVolume();

  return scene;
}

std::vector<std::string> Scene::SplitString(const std::string& s, char delim) {
  std::vector<std::string> elems(0);
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.emplace_back(item);
  }
  return elems;
}

std::shared_ptr<Mesh> Scene::ParseMesh(
    const std::vector<std::string>& mesh_texts) {
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvs;

  for (const auto& line : mesh_texts) {
    auto texts = SplitString(line, ' ');

    if (texts[0] == "v") {
      vertices.emplace_back(std::stof(texts[1]), std::stof(texts[3]),
                            -std::stof(texts[2]));
    } else if (texts[0] == "vn") {
      normals.emplace_back(std::stof(texts[1]), std::stof(texts[3]),
                           -std::stof(texts[2]));
    } else if (texts[0] == "vt") {
      uvs.emplace_back(std::stof(texts[1]), std::stof(texts[2]));
    }
  }

  return std::make_shared<Mesh>(vertices, normals, uvs);
}

std::shared_ptr<Material> Scene::ParseMaterial(
    const std::string& path, const std::vector<std::string>& material_texts) {
  Texture albedo_texture;
  Texture metallic_texture;
  Texture roughness_texture;
  Texture emissive_texture;
  Texture normal_map_texture;
  GLfloat emissive_intensity;

  for (const auto& line : material_texts) {
    auto texts = SplitString(line, ' ');

    if (texts[0] == "baseColor:") {
      albedo_texture = Texture(path + "/" + texts[1], true);
    } else if (texts[0] == "metallic:") {
      metallic_texture = Texture(path + "/" + texts[1], false);
    } else if (texts[0] == "roughness:") {
      roughness_texture = Texture(path + "/" + texts[1], false);
    } else if (texts[0] == "emission:") {
      emissive_texture = Texture(path + "/" + texts[1], true);
    } else if (texts[0] == "normalMap:") {
      normal_map_texture = Texture(path + "/" + texts[1], false);
    } else if (texts[0] == "emissionIntensity:") {
      emissive_intensity = std::stof(texts[1]);
    }
  }

  return std::make_shared<Material>(
      std::move(albedo_texture), std::move(metallic_texture),
      std::move(roughness_texture), std::move(normal_map_texture),
      std::move(emissive_texture), emissive_intensity);
}

}  // namespace game
