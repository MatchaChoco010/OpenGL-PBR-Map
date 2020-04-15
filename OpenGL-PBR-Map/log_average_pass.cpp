#include "log_average_pass.h"

namespace game {

void LogAveragePass::Render() {
  glUseProgram(shader_program_);
  glBindFramebuffer(GL_FRAMEBUFFER, log_average_fbo_);

  glDisable(GL_STENCIL_TEST);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdr_color_buffer_);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(fullscreen_mesh_vao_);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glBindTexture(GL_TEXTURE_2D, log_average_buffer_);
  glGenerateMipmap(GL_TEXTURE_2D);

  const int level = static_cast<int>(std::log2(std::max(width_, height_)));
  GLfloat pixel[3];
  glGetTexImage(GL_TEXTURE_2D, level, GL_RGB, GL_FLOAT, &pixel);
  l_average_ = std::expf(pixel[0]);

  glBindTexture(GL_TEXTURE_2D, 0);
}

const GLfloat LogAveragePass::GetLLogAverage() const { return l_average_; }

LogAveragePass::LogAveragePass(const GLuint hdr_color_buffer,
                               const GLuint fullscreen_mesh_vao,
                               const GLuint width, const GLuint height)
    : width_(width),
      height_(height),

      hdr_color_buffer_(hdr_color_buffer),
      fullscreen_mesh_vao_(fullscreen_mesh_vao),

      log_average_buffer_(CreateLogAverageBuffer(width, height)),
      log_average_fbo_(CreateLogAverageFbo(log_average_buffer_)),

      shader_program_(CreateProgram("shader/LogAveragePass.vert",
                                    "shader/LogAveragePass.frag")),
      l_average_(0) {}

LogAveragePass::~LogAveragePass() {
  glDeleteFramebuffers(1, &log_average_fbo_);
  glDeleteTextures(1, &log_average_buffer_);

  glDeleteProgram(shader_program_);
}

const GLuint LogAveragePass::CreateLogAverageBuffer(const GLuint width,
                                                    const GLuint height) {
  GLuint log_average_buffer;
  glGenTextures(1, &log_average_buffer);
  glBindTexture(GL_TEXTURE_2D, log_average_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT,
               nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  return log_average_buffer;
}

const GLuint LogAveragePass::CreateLogAverageFbo(
    const GLuint log_average_buffer) {
  GLuint log_average_fbo;
  glGenFramebuffers(1, &log_average_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, log_average_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         log_average_buffer, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return log_average_fbo;
}

}  // namespace game