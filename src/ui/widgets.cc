#include "ui/widgets.h"

#include <string>
#include <utility>
#include <vector>

#include "ftxui/dom/elements.hpp"

ftxui::Element RenderKeyHints(const std::vector<KeyHint>& hints) {
  ftxui::Elements parts;
  for (const KeyHint& hint : hints) {
    if (!parts.empty()) {
      parts.push_back(ftxui::text("    "));
    }
    parts.push_back(ftxui::text(hint.keys) | ftxui::bold);
    parts.push_back(ftxui::text(" " + hint.action) | ftxui::dim);
  }
  return ftxui::hbox(std::move(parts));
}
