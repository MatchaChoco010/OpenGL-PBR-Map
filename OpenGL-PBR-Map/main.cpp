#include "application.h"

int main() {
  game::Application app;
  return app.Run() ? 0 : -1;
}