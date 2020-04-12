#ifndef OPENGL_PBR_MAP_EXR_TEXTURE_H_
#define OPENGL_PBR_MAP_EXR_TEXTURE_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

namespace game {

/**
 * @brief .exr形式のテクスチャ
 *
 * OpenEXR形式のテクスチャを読み込みOpenGLにアップロードするクラスです。
 * テクスチャの多重開放を避けるためコピー禁止です。
 * ムーブは可能です。
 * tinyexrを利用しています。
 * https://github.com/syoyo/tinyexr
 */
class ExrTexture {
 public:
  /**
   * @brief テクスチャの幅を取得
   * @return テクスチャの幅
   */
  const int GetWidth() const;

  /**
   * @brief テクスチャの高さを取得
   * @return テクスチャの高さ
   */
  const int GetHeight() const;

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
  ExrTexture();

  /**
   * @brief コンストラクタ
   * @param path テクスチャのパス
   * @param sRGB sRGBならばtrue、Linearならばfalse
   */
  ExrTexture(const std::string& path);

  /**
   * @brief デストラクタ
   *
   * texture_id_のテクスチャを開放します。
   */
  ~ExrTexture();

  // コピー禁止
  ExrTexture(const ExrTexture&) = delete;
  ExrTexture& operator=(const ExrTexture&) = delete;

  /**
   * @brief ムーブコンストラクタ
   * @param other ムーブ元
   *
   * ムーブ後のtexture_id_は0に初期化されます。
   */
  ExrTexture(ExrTexture&& other) noexcept;

  /**
   * @brief ムーブ代入演算子
   * @param other ムーブ元
   *
   * ムーブ後のtexture_id_は0に初期化されます。
   */
  ExrTexture& operator=(ExrTexture&& other) noexcept;

 private:
  int width_;
  int height_;
  int channel_;
  GLuint texture_id_;
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_EXR_TEXTURE_H_