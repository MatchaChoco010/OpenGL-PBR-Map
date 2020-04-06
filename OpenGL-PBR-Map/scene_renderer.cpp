#include "scene_renderer.h"

namespace game {

void SceneRenderer::Render(const Scene& scene, const double delta_time) {
  geometry_pass_.Render(scene);

  // HDRカラーバッファにDepthとStencilの転写
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer_fbo_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdr_fbo_);
  glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_,
                    GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

  directional_light_pass_.Render(scene);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, hdr_fbo_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

SceneRenderer::SceneRenderer(const GLuint width, const GLuint height)
    : width_(width),
      height_(height),
      fullscreen_mesh_vao_(CreateFullscreenMeshVao()),
      fullscreen_mesh_vertices_vbo_(
          CreateFullscreenMeshVerticesVbo(fullscreen_mesh_vao_)),
      fullscreen_mesh_uvs_vbo_(
          CreateFullscreenMeshUvsVbo(fullscreen_mesh_vao_)),
      gbuffer0_(CreateGBuffer0(width, height)),
      gbuffer1_(CreateGBuffer1(width, height)),
      gbuffer2_(CreateGBuffer2(width, height)),
      gbuffer_depth_(CreateGBufferDepth(width, height)),
      gbuffer_fbo_(
          CreateGBufferFbo(gbuffer0_, gbuffer1_, gbuffer2_, gbuffer_depth_)),
      hdr_color_buffer_(CreateHdrColorBuffer(width, height)),
      hdr_depth_buffer_(CreateHdrDepthBuffer(width, height)),
      hdr_fbo_(CreateHdrFbo(hdr_color_buffer_, hdr_depth_buffer_)),
      geometry_pass_(gbuffer_fbo_),
      directional_light_pass_(hdr_fbo_, gbuffer0_, gbuffer1_, gbuffer2_,
                              fullscreen_mesh_vao_, width, height) {}

SceneRenderer::~SceneRenderer() { Release(); }

void SceneRenderer::Release() {
  glDeleteFramebuffers(1, &gbuffer_fbo_);
  glDeleteTextures(1, &gbuffer0_);
  glDeleteTextures(1, &gbuffer1_);
  glDeleteTextures(1, &gbuffer2_);
  glDeleteRenderbuffers(1, &gbuffer_depth_);
}

const GLuint SceneRenderer::CreateFullscreenMeshVao() {
  GLuint fullscreen_mesh_vao;
  glGenVertexArrays(1, &fullscreen_mesh_vao);
  return fullscreen_mesh_vao;
}

const GLuint SceneRenderer::CreateFullscreenMeshVerticesVbo(
    const GLuint fullscreen_mesh_vao) {
  glBindVertexArray(fullscreen_mesh_vao);

  const std::array<glm::vec2, 3> fullscreen_mesh_vertices = {
      glm::vec2(-1.0, -1.0),
      glm::vec2(3.0, -1.0),
      glm::vec2(-1.0, 3.0),
  };

  GLuint fullscreen_mesh_vertices_vbo;
  glGenBuffers(1, &fullscreen_mesh_vertices_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, fullscreen_mesh_vertices_vbo);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2),
               &fullscreen_mesh_vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  return fullscreen_mesh_vertices_vbo;
}

const GLuint SceneRenderer::CreateFullscreenMeshUvsVbo(
    const GLuint fullscreen_mesh_vao) {
  glBindVertexArray(fullscreen_mesh_vao);

  const std::array<glm::vec2, 3> fullscreen_mesh_uvs = {
      glm::vec2(0.0, 0.0), glm::vec2(2.0, 0.0), glm::vec2(0.0, 2.0)};

  GLuint fullscreen_mesh_uvs_vbo;
  glGenBuffers(1, &fullscreen_mesh_uvs_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, fullscreen_mesh_uvs_vbo);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2), &fullscreen_mesh_uvs[0],
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  return fullscreen_mesh_uvs_vbo;
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

const GLuint SceneRenderer::CreateHdrColorBuffer(const GLuint width,
                                                 const GLuint height) {
  GLuint hdr_color_buffer;
  glGenTextures(1, &hdr_color_buffer);
  glBindTexture(GL_TEXTURE_2D, hdr_color_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  return hdr_color_buffer;
}

const GLuint SceneRenderer::CreateHdrDepthBuffer(const GLuint width,
                                                 const GLuint height) {
  GLuint hdr_depth_buffer;
  glGenTextures(1, &hdr_depth_buffer);
  glBindTexture(GL_TEXTURE_2D, hdr_depth_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
               GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE,
                  GL_DEPTH_COMPONENT);
  glBindTexture(GL_TEXTURE_2D, 0);
  return hdr_depth_buffer;
}

const GLuint SceneRenderer::CreateHdrFbo(const GLuint hdr_color_buffer,
                                         const GLuint hdr_depth_buffer) {
  GLuint hdr_fbo;
  glGenFramebuffers(1, &hdr_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         hdr_color_buffer, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         GL_TEXTURE_2D, hdr_depth_buffer, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return hdr_fbo;
}

}  // namespace game
