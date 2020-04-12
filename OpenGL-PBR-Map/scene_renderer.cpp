#include "scene_renderer.h"

namespace game {

void SceneRenderer::Render(const Scene& scene, const double delta_time) {
  geometry_pass_.Render(scene);

  // HDRカラーバッファにDepthとStencilの転写
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer_fbo_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdr_fbo_);
  glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_,
                    GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

  sky_pass_.Render(scene);

  directional_light_pass_.Render(scene);

  point_light_pass_.Render(scene);

  spot_light_pass_.Render(scene);

  exposure_pass_.SetExposure(0.001f);
  exposure_pass_.Render();

  tonemapping_pass_.Render();
}

SceneRenderer::SceneRenderer(const GLuint width, const GLuint height)
    : width_(width),
      height_(height),

      fullscreen_mesh_vao_(CreateFullscreenMeshVao()),
      fullscreen_mesh_vertices_vbo_(
          CreateFullscreenMeshVerticesVbo(fullscreen_mesh_vao_)),
      fullscreen_mesh_uvs_vbo_(
          CreateFullscreenMeshUvsVbo(fullscreen_mesh_vao_)),

      sphere_vao_(CreateSphereMeshVao()),
      sphere_vertices_vbo_(CreateSphereMeshVbo(sphere_vao_)),
      sphere_indices_ibo_(CreateSphereMeshIbo(sphere_vao_)),

      gbuffer0_(CreateGBuffer0(width, height)),
      gbuffer1_(CreateGBuffer1(width, height)),
      gbuffer2_(CreateGBuffer2(width, height)),
      gbuffer_depth_(CreateGBufferDepth(width, height)),
      gbuffer_fbo_(
          CreateGBufferFbo(gbuffer0_, gbuffer1_, gbuffer2_, gbuffer_depth_)),

      hdr_color_buffer_(CreateHdrColorBuffer(width, height)),
      hdr_depth_buffer_(CreateHdrDepthBuffer(width, height)),
      hdr_fbo_(CreateHdrFbo(hdr_color_buffer_, hdr_depth_buffer_)),

      exposured_color_buffer_(CreateExposuredColorBuffer(width, height)),
      exposured_fbo_(CreateExposuredFbo(exposured_color_buffer_)),

      geometry_pass_(gbuffer_fbo_),
      sky_pass_(hdr_fbo_, fullscreen_mesh_vao_, width_, height_),
      directional_light_pass_(hdr_fbo_, gbuffer0_, gbuffer1_, gbuffer2_,
                              fullscreen_mesh_vao_, width, height),
      point_light_pass_(hdr_fbo_, gbuffer0_, gbuffer1_, gbuffer2_, sphere_vao_,
                        width, height),
      spot_light_pass_(hdr_fbo_, gbuffer0_, gbuffer1_, gbuffer2_, sphere_vao_,
                       width, height),
      exposure_pass_(hdr_color_buffer_, exposured_fbo_, fullscreen_mesh_vao_,
                     width, height),
      tonemapping_pass_(exposured_color_buffer_, fullscreen_mesh_vao_, width,
                        height) {}

SceneRenderer::~SceneRenderer() { Release(); }

void SceneRenderer::Release() {
  glDeleteVertexArrays(1, &fullscreen_mesh_vao_);
  glDeleteBuffers(1, &fullscreen_mesh_vertices_vbo_);
  glDeleteBuffers(1, &fullscreen_mesh_uvs_vbo_);

  glDeleteVertexArrays(1, &sphere_vao_);
  glDeleteBuffers(1, &sphere_vertices_vbo_);
  glDeleteBuffers(1, &sphere_indices_ibo_);

  glDeleteFramebuffers(1, &gbuffer_fbo_);
  glDeleteTextures(1, &gbuffer0_);
  glDeleteTextures(1, &gbuffer1_);
  glDeleteTextures(1, &gbuffer2_);
  glDeleteRenderbuffers(1, &gbuffer_depth_);

  glDeleteFramebuffers(1, &hdr_fbo_);
  glDeleteTextures(1, &hdr_color_buffer_);
  glDeleteRenderbuffers(1, &hdr_depth_buffer_);

  glDeleteFramebuffers(1, &exposured_fbo_);
  glDeleteTextures(1, &exposured_color_buffer_);
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

const GLuint SceneRenderer::CreateSphereMeshVao() {
  GLuint sphere_vao;
  glGenVertexArrays(1, &sphere_vao);
  return sphere_vao;
}

const GLuint SceneRenderer::CreateSphereMeshVbo(const GLuint sphere_mesh_vao) {
  glBindVertexArray(sphere_mesh_vao);

  const int slices = 8, stacks = 8;
  std::vector<glm::vec3> sphere_vertices;
  for (int j = 0; j <= stacks; ++j) {
    const float t(static_cast<float>(j) / static_cast<float>(stacks));
    const float y(cos(3.141593f * t)), r(sin(3.141593f * t));
    for (int i = 0; i <= slices; ++i) {
      const float s(static_cast<float>(i) / static_cast<float>(slices));
      const float z(r * cos(6.283185f * s)), x(r * sin(6.283185f * s));
      sphere_vertices.emplace_back(x, y, z);
    }
  }

  GLuint sphere_vertices_vbo;
  glGenBuffers(1, &sphere_vertices_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_vertices_vbo);
  glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(glm::vec3),
               &sphere_vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  glBindVertexArray(0);

  return sphere_vertices_vbo;
}

const GLuint SceneRenderer::CreateSphereMeshIbo(const GLuint sphere_mesh_vao) {
  glBindVertexArray(sphere_mesh_vao);

  const int slices = 8, stacks = 8;
  std::vector<GLuint> sphere_indices;
  for (int j = 0; j < stacks; ++j) {
    const int k((slices + 1) * j);
    for (int i = 0; i < slices; ++i) {
      const GLuint k0(k + i);
      const GLuint k1(k0 + 1);
      const GLuint k2(k1 + slices);
      const GLuint k3(k2 + 1);

      sphere_indices.emplace_back(k0);
      sphere_indices.emplace_back(k2);
      sphere_indices.emplace_back(k3);

      sphere_indices.emplace_back(k0);
      sphere_indices.emplace_back(k3);
      sphere_indices.emplace_back(k1);
    }
  }

  GLuint sphere_indices_ibo;
  glGenBuffers(1, &sphere_indices_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_indices_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.size() * sizeof(GLuint),
               &sphere_indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);

  return sphere_indices_ibo;
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

const GLuint SceneRenderer::CreateExposuredColorBuffer(const GLuint width,
                                                       const GLuint height) {
  GLuint exposured_color_buffer;
  glGenTextures(1, &exposured_color_buffer);
  glBindTexture(GL_TEXTURE_2D, exposured_color_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  return exposured_color_buffer;
}

const GLuint SceneRenderer::CreateExposuredFbo(
    const GLuint exposured_color_buffer) {
  GLuint exposured_fbo;
  glGenFramebuffers(1, &exposured_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, exposured_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         exposured_color_buffer, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return exposured_fbo;
}

}  // namespace game
