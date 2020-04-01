#ifndef OPENGL_PBR_MAP_TEXTURE_H_
#define OPENGL_PBR_MAP_TEXTURE_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

namespace game {

/**
 * @brief Textureを表現するオブジェクト
 *
 * OpenGLのTextureを生成しIDを管理します。
 * テクスチャの多重開放を避けるためコピー禁止です。
 * ムーブは可能です。
 * テクスチャの読み込みにはstbのstb_image.hを利用しています。
 * https://github.com/nothings/stb
 */
class Texture {
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
   * @brief テクスチャのチャンネル数を取得
   * @return テクスチャのチャンネル数
   */
  const int GetChannel() const;

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
  Texture();

  /**
   * @brief コンストラクタ
   * @param path テクスチャのパス
   * @param sRGB sRGBならばtrue、Linearならばfalse
   */
  Texture(const std::string& path, const bool sRGB);

  /**
   * @brief デストラクタ
   *
   * texture_id_のテクスチャを開放します。
   */
  ~Texture();

  // コピー禁止
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  /**
   * @brief ムーブコンストラクタ
   * @param other ムーブ元
   *
   * ムーブ後のtexture_id_は0に初期化されます。
   */
  Texture(Texture&& other) noexcept;

  /**
   * @brief ムーブ代入演算子
   * @param other ムーブ元
   *
   * ムーブ後のtexture_id_は0に初期化されます。
   */
  Texture& operator=(Texture&& other) noexcept;

 private:
  int width_;
  int height_;
  int channel_;
  GLuint texture_id_;
};

}  // namespace game

#endif  // OPENGL_PBR_MAP_TEXTURE_H_