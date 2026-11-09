#pragma once

#include <optional>
#include <string>
#include <utility>

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/screen/screen.hpp"

constexpr int kScreenWidth = 80;
constexpr int kScreenHeight = 24;

struct CellPosition {
  int x;
  int y;
};

// Searches cell by cell because Screen::ToString() mixes style escape codes
// into the text. Supports ASCII text only, where each character fills one cell.
inline std::optional<CellPosition> FindText(const ftxui::Screen& screen,
                                            const std::string& text) {
  const int length = static_cast<int>(text.size());
  for (int y = 0; y < screen.dimy(); ++y) {
    for (int x = 0; x + length <= screen.dimx(); ++x) {
      bool matches = true;
      for (int i = 0; i < length && matches; ++i) {
        matches = screen.CellAt(x + i, y).character == std::string(1, text[i]);
      }
      if (matches) {
        return CellPosition{x, y};
      }
    }
  }
  return std::nullopt;
}

inline ftxui::Screen Draw(const ftxui::Element& element) {
  ftxui::Screen screen(kScreenWidth, kScreenHeight);
  ftxui::Render(screen, element);
  return screen;
}

// Components need a frame drawn before input, because mouse handling uses
// where the last frame placed each part.
inline ftxui::Screen Draw(const ftxui::Component& component) {
  return Draw(component->Render());
}

inline bool Shows(const ftxui::Component& component, const std::string& text) {
  return FindText(Draw(component), text).has_value();
}

inline void Press(const ftxui::Component& component, ftxui::Event event) {
  Draw(component);
  component->OnEvent(std::move(event));
}

// Returns false if label is not drawn.
inline bool Click(const ftxui::Component& component, const std::string& label) {
  std::optional<CellPosition> position = FindText(Draw(component), label);
  if (!position.has_value()) {
    return false;
  }
  ftxui::Mouse mouse;
  mouse.button = ftxui::Mouse::Left;
  mouse.x = position->x;
  mouse.y = position->y;
  mouse.motion = ftxui::Mouse::Pressed;
  component->OnEvent(ftxui::Event::Mouse("", mouse));
  mouse.motion = ftxui::Mouse::Released;
  component->OnEvent(ftxui::Event::Mouse("", mouse));
  return true;
}
