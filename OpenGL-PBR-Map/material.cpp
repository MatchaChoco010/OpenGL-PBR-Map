#include "material.h"

namespace game {

Material::Material(Texture&& albedo_map, Texture&& metallic_map,
                   Texture&& roughness_map, Texture&& normal_map,
                   Texture&& emissive_map, const GLfloat emissive_intensity)
    : albedo_map_(std::move(albedo_map)),
      metallic_map_(std::move(metallic_map)),
      roughness_map_(std::move(roughness_map)),
      normal_map_(std::move(normal_map)),
      emissive_map_(std::move(emissive_map)),
      emissive_intensity_(emissive_intensity) {}

}  // namespace game