#include "ftxui/component/screen_interactive.hpp"
#include "ui/app.h"

int main() {
  ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
  App app(screen.ExitLoopClosure());
  screen.Loop(app.Root());
  return 0;
}
