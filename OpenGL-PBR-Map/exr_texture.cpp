#include "exr_texture.h"

#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>

namespace game {

const int ExrTexture::GetWidth() const { return width_; }

const int ExrTexture::GetHeight() const { return height_; }

const GLuint ExrTexture::GetTextureId() const { return texture_id_; }

ExrTexture::ExrTexture() : width_(0), height_(0), channel_(0), texture_id_(0) {}

ExrTexture::ExrTexture(const std::string& path) : channel_(4), texture_id_(0) {
  float* data;
  const char* err = nullptr;
  int ret = LoadEXR(&data, &width_, &height_, path.c_str(), &err);
  if (ret != TINYEXR_SUCCESS) {
    std::cerr << "Can't load image: " << path << std::endl;
    if (err) {
      std::cerr << "ERR : " << err << std::endl;
      FreeEXRErrorMessage(err);
    }
  } else {
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA,
                 GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

ExrTexture::~ExrTexture() { glDeleteTextures(1, &texture_id_); }

ExrTexture::ExrTexture(ExrTexture&& other) noexcept
    : texture_id_(other.texture_id_),
      width_(other.width_),
      height_(other.height_),
      channel_(other.channel_) {
  other.texture_id_ = 0;
}

ExrTexture& ExrTexture::operator=(ExrTexture&& other) noexcept {
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

}