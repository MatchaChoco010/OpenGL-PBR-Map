#include "scene_renderer.h"

namespace game {

void SceneRenderer::Render(const Scene& scene, const double delta_time) {
  geometry_pass_.Render(scene);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer_fbo_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  // glReadBuffer(GL_COLOR_ATTACHMENT0);
  // glReadBuffer(GL_COLOR_ATTACHMENT1);
  glReadBuffer(GL_COLOR_ATTACHMENT2);
  glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

SceneRenderer::SceneRenderer(const GLuint width, const GLuint height)
    : width_(width),
      height_(height),
      gbuffer0_(CreateGBuffer0(width, height)),
      gbuffer1_(CreateGBuffer1(width, height)),
      gbuffer2_(CreateGBuffer2(width, height)),
      gbuffer_depth_(CreateGBufferDepth(width, height)),
      gbuffer_fbo_(
          CreateGBufferFbo(gbuffer0_, gbuffer1_, gbuffer2_, gbuffer_depth_)),
      geometry_pass_(gbuffer_fbo_) {}

SceneRenderer::~SceneRenderer() { Release(); }

void SceneRenderer::Release() {
  glDeleteFramebuffers(1, &gbuffer_fbo_);
  glDeleteTextures(1, &gbuffer0_);
  glDeleteTextures(1, &gbuffer1_);
  glDeleteTextures(1, &gbuffer2_);
  glDeleteRenderbuffers(1, &gbuffer_depth_);
}

const GLuint SceneRenderer::CreateGBuffer0(const GLuint width,
                                           const GLuint height) {
  GLuint gbuffer0;
  glGenTextures(1, &gbuffer0);
  glBindTexture(GL_TEXTURE_2D, gbuffer0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  return gbuffer0;
}

const GLuint SceneRenderer::CreateGBuffer1(const GLuint width,
                                           const GLuint height) {
  GLuint gbuffer1;
  glGenTextures(1, &gbuffer1);
  glBindTexture(GL_TEXTURE_2D, gbuffer1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  return gbuffer1;
}

const GLuint SceneRenderer::CreateGBuffer2(const GLuint width,
                                           const GLuint height) {
  GLuint gbuffer2;
  glGenTextures(1, &gbuffer2);
  glBindTexture(GL_TEXTURE_2D, gbuffer2);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  return gbuffer2;
}

const GLuint SceneRenderer::CreateGBufferDepth(const GLuint width,
                                               const GLuint height) {
  GLuint gbuffer_depth_buffer;
  glGenRenderbuffers(1, &gbuffer_depth_buffer);
  glBindRenderbuffer(GL_RENDERBUFFER, gbuffer_depth_buffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  return gbuffer_depth_buffer;
}

const GLuint SceneRenderer::CreateGBufferFbo(const GLuint gbuffer0,
                                             const GLuint gbuffer1,
                                             const GLuint gbuffer2,
                                             const GLuint gbuffer_depth) {
  GLuint gbuffer_fbo;
  glGenFramebuffers(1, &gbuffer_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gbuffer0, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gbuffer1, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gbuffer2, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, gbuffer_depth);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return gbuffer_fbo;
}

}  // namespace game
