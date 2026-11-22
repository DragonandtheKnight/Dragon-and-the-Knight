#include <random>

#include "ftxui/component/screen_interactive.hpp"
#include "ui/app.h"

int main() {
  ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
  std::random_device seed;
  App app(screen.ExitLoopClosure(), seed());
  screen.Loop(app.Root());
  return 0;
}
