#include "exr_texture.h"

#include "exr_cubemap_texture.h"
#include "specular_ibl_exr_cubemap_texture.h"

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

const GLuint ExrCubemapTexture::GetTextureId() const { return texture_id_; }

ExrCubemapTexture::ExrCubemapTexture() : texture_id_(0) {}

ExrCubemapTexture::ExrCubemapTexture(const std::string& path) : texture_id_(0) {
  float* data;
  const char* err = nullptr;
  int width, height;
  std::array<std::string, 6> filenames = {"pos-x.exr", "neg-x.exr",
                                          "pos-y.exr", "neg-y.exr",
                                          "pos-z.exr", "neg-z.exr"};

  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  for (unsigned int i = 0; i < 6; ++i) {
    const auto img_path = path + "/" + filenames[i];
    int ret = LoadEXR(&data, &width, &height, img_path.c_str(), &err);
    if (ret != TINYEXR_SUCCESS) {
      std::cerr << "Can't load image: " << img_path << std::endl;
      if (err) {
        std::cerr << "ERR : " << err << std::endl;
        FreeEXRErrorMessage(err);
      }
    } else {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, width,
                   height, 0, GL_RGBA, GL_FLOAT, data);
      free(data);
    }
  }
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

ExrCubemapTexture::~ExrCubemapTexture() { glDeleteTextures(1, &texture_id_); }

ExrCubemapTexture::ExrCubemapTexture(ExrCubemapTexture&& other) noexcept
    : texture_id_(other.texture_id_) {
  other.texture_id_ = 0;
}

ExrCubemapTexture& ExrCubemapTexture::operator=(
    ExrCubemapTexture&& other) noexcept {
  if (this != &other) {
    glDeleteTextures(1, &texture_id_);
    texture_id_ = other.texture_id_;
    other.texture_id_ = 0;
  }

  return *this;
}

const GLuint SpecularIblExrCubemapTexture::GetTextureId() const {
  return texture_id_;
}

SpecularIblExrCubemapTexture::SpecularIblExrCubemapTexture() : texture_id_(0) {}

SpecularIblExrCubemapTexture::SpecularIblExrCubemapTexture(
    const std::string& path)
    : texture_id_(0) {
  float* data;
  const char* err = nullptr;
  int width, height;

  const std::array<std::string, 5> dirs = {
      path + "/roughness-0",   path + "/roughness-0-25",
      path + "/roughness-0-5", path + "/roughness-0-75",
      path + "/roughness-1",
  };

  std::array<std::string, 6> filenames = {"pos-x.exr", "neg-x.exr",
                                          "pos-y.exr", "neg-y.exr",
                                          "pos-z.exr", "neg-z.exr"};

  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
  for (unsigned int j = 0; j < 5; ++j) {
    for (unsigned int i = 0; i < 6; ++i) {
      const auto img_path = dirs[j] + "/" + filenames[i];
      int ret = LoadEXR(&data, &width, &height, img_path.c_str(), &err);
      if (ret != TINYEXR_SUCCESS) {
        std::cerr << "Can't load image: " << img_path << std::endl;
        if (err) {
          std::cerr << "ERR : " << err << std::endl;
          FreeEXRErrorMessage(err);
        }
      } else {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, GL_RGBA32F, width,
                     height, 0, GL_RGBA, GL_FLOAT, data);
        free(data);
      }
    }
    if (j == 0) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

SpecularIblExrCubemapTexture::~SpecularIblExrCubemapTexture() {
  glDeleteTextures(1, &texture_id_);
}

SpecularIblExrCubemapTexture::SpecularIblExrCubemapTexture(
    SpecularIblExrCubemapTexture&& other) noexcept
    : texture_id_(other.texture_id_) {
  other.texture_id_ = 0;
}

SpecularIblExrCubemapTexture& SpecularIblExrCubemapTexture::operator=(
    SpecularIblExrCubemapTexture&& other) noexcept {
  if (this != &other) {
    glDeleteTextures(1, &texture_id_);
    texture_id_ = other.texture_id_;
    other.texture_id_ = 0;
  }

  return *this;
}

}  // namespace game