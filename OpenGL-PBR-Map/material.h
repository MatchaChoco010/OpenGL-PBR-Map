#ifndef OPENGL_PBR_MAP_MATERIAL_H_
#define OPENGL_PBR_MAP_MATERIAL_H_

#include "texture.h"

namespace game {

/**
 * @brief マテリアルを表現するオブジェクト
 *
 * PBRマテリアルに必要な各種テクスチャとEmissiveの強さを保持します。
 */
class Material {
 public:
  const Texture albedo_map_;
  const Texture metallic_map_;
  const Texture roughness_map_;
  const Texture normal_map_;
  const Texture emissive_map_;
  const GLfloat emissive_intensity_;

  /**
   * @brief コンストラクタ
   * @param albedo_map アルベドテクスチャ
   * @param metallic_map メタリックテクスチャ
   * @param roughnness_map ラフネステクスチャ
   * @param normal_map ノーマルマップテクスチャ
   * @param emissive_map エミッシブテクスチャ
   * @param emissive_intensity エミッシブの強さ
   *
   * 各種テクスチャとEmissiveの強さを指定してインスタンスを作成します。
   */
  Material(Texture&& albedo_map, Texture&& metallic_map,
           Texture&& roughness_map, Texture&& normal_map,
           Texture&& emissive_map, const GLfloat emissive_intensity);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_MATERIAL_H_
