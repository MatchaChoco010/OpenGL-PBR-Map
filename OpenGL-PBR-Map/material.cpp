#include "material.h"

namespace game {

Material::Material(Texture&& albedo_map, Texture&& normal_map)
    : albedo_map_(std::move(albedo_map)), normal_map_(std::move(normal_map)) {}

}  // namespace game