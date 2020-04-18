#ifndef OPENGL_PBR_MAP_SPECULAR_IBL_EXR_CUBEMAP_TEXTURE_H_
#define OPENGL_PBR_MAP_SPECULAR_IBL_EXR_CUBEMAP_TEXTURE_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <string>

namespace game {

/**
 * @brief SpecularIBL用のCubemapテクスチャ
 *
 * OpenEXR形式のテクスチャを読み込みOpenGLにアップロードするクラスです。
 * テクスチャの多重開放を避けるためコピー禁止です。
 * ムーブは可能です。
 * tinyexrを利用しています。
 * https://github.com/syoyo/tinyexr
 */
class SpecularIblExrCubemapTexture {
 public:
  /**
   * @brief テクスチャIDを取得
   * @return テクスチャID
   */
  const GLuint GetTextureId() const;

  /**
   * @brief デフォルトコンストラクタ
   *
   * 何もファイルを読み込まない空のテクスチャとなります。
   * texture_id_は0で初期化されます。
   */
  SpecularIblExrCubemapTexture();

  /**
   * @brief コンストラクタ
   * @param path Cubemapテクスチャのディレクトリのパス
   */
  SpecularIblExrCubemapTexture(const std::string& path);

  /**
   * @brief デストラクタ
   *
   * texture_id_のテクスチャを開放します。
   */
  ~SpecularIblExrCubemapTexture();

  // コピー禁止
  SpecularIblExrCubemapTexture(const SpecularIblExrCubemapTexture&) = delete;
  SpecularIblExrCubemapTexture& operator=(const SpecularIblExrCubemapTexture&) =
      delete;

  /**
   * @brief ムーブコンストラクタ
   * @param other ムーブ元
   *
   * ムーブ後のtexture_id_は0に初期化されます。
   */
  SpecularIblExrCubemapTexture(SpecularIblExrCubemapTexture&& other) noexcept;

  /**
   * @brief ムーブ代入演算子
   * @param other ムーブ元
   *
   * ムーブ後のtexture_id_は0に初期化されます。
   */
  SpecularIblExrCubemapTexture& operator=(
      SpecularIblExrCubemapTexture&& other) noexcept;

 private:
  GLuint texture_id_;
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_SPECULAR_IBL_EXR_CUBEMAP_TEXTURE_H_