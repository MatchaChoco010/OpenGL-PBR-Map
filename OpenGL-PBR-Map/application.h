#ifndef OPENGL_PBR_MAP_APPLICATION_H_
#define OPENGL_PBR_MAP_APPLICATION_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include "camera.h"
#include "mesh.h"
#include "mesh_entity.h"

namespace game {

class Application {
 public:
  /**
   * @brief アプリケーションのエントリーポイント
   * @return 正常終了ならばtrue、異常終了ならばfalse
   *
   * アプリケーションのエントリーポイントです。
   * 内部でループを回しゲームを実行します。
   * ウィンドウが閉じられた場合、ゲームが終了した場合、
   * 何かしらのエラーで停止した場合などにこのメソッドが終了します。
   * 戻り値は正常終了ならばtrue、異常終了ならばfalseです。
   *
   * ```
   * int main() {
   *     game::Application app;
   *     return app.Run() ? 0 : -1;
   * }
   * ```
   */
  bool Run();

 private:
  GLFWwindow* window_;
  GLuint program_;
  GLuint model_loc_;
  GLuint view_projection_loc_;
  std::vector<MeshEntity> mesh_entities_;
  std::unique_ptr<Camera> camera_;

  /**
   * @brief 初期化処理
   * @return 正常終了ならばtrue、異常終了ならばfalse
   */
  bool Init();

  /**
   * @brief GLFWのウィンドウを作成する
   * @param width ウィンドウの幅
   * @param height ウィンドウの高さ
   * @return 正常終了ならばtrue、異常終了ならばfalse
   *
   * GLFWの初期化とウィンドウ作成、GLEWの初期化、
   * その他GLの初期設定を行います。
   */
  bool InitWindow(const GLuint width, const GLuint height);

  /**
   * @brief 毎フレーム呼ばれる処理
   * @param delta_time 前フレームとの差分時間（秒）
   */
  void Update(const double delta_time);
};

}  // namespace game

#endif  //  OPENGL_PBR_MAP_APPLICATION_H_