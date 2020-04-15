#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace game {

const int Texture::GetWidth() const { return width_; }

const int Texture::GetHeight() const { return height_; }

const int Texture::GetChannel() const { return channel_; }

const GLuint Texture::GetTextureId() const { return texture_id_; }

Texture::Texture() : width_(0), height_(0), channel_(0), texture_id_(0) {}

Texture::Texture(const std::string& path, const bool sRGB) {
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data =
      stbi_load(path.c_str(), &width_, &height_, &channel_, 0);

  if (!data) {
    std::cerr << "Can't load image: " << path << std::endl;
    texture_id_ = 0;
  } else {
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    if (sRGB) {
      if (channel_ == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width_, height_, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);
      } else if (channel_ == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width_, height_, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
      }
    } else {
      if (channel_ == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
      } else if (channel_ == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
      } else if (channel_ == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width_, height_, 0, GL_RED,
                     GL_UNSIGNED_BYTE, data);
      }
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

Texture::~Texture() { glDeleteTextures(1, &texture_id_); }

Texture::Texture(Texture&& other) noexcept
    : texture_id_(other.texture_id_),
      width_(other.width_),
      height_(other.height_),
      channel_(other.channel_) {
  other.texture_id_ = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
  if (this != &other) {
    glDeleteTextures(1, &texture_id_);
    texture_id_ = other.texture_id_;
    other.texture_id_ = 0;

    width_ = other.width_;
    height_ = other.height_;
    channel_ = other.channel_;
  }

  return *this;
}

}  // namespace game