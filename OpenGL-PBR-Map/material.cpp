#include "material.h"

namespace game {

Material::Material(Texture&& albedo_map)
    : albedo_map_(std::move(albedo_map)) {}

}  // namespace game