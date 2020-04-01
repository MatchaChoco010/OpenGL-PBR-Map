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
  const Texture normal_map_;

  /**
   * @brief コンストラクタ
   * @param albedo_map アルベドテクスチャ
   * @param normal_map ノーマルマップテクスチャ
   *
   * 各種テクスチャを指定してインスタンスを作成します。
   */
  Material(Texture&& albedo_map, Texture&& normal_map);
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_MATERIAL_H_
