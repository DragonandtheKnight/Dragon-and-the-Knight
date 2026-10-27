# Contributing

## When contributing to this repository, please follow these step:

1. **Create a new branch** for the feature or bug fix.
2. Open a **Pull Request** and **Follow** [**Conventional Commits**](https://www.conventionalcommits.org/) Rules.
3. **Review** your change and make sure it PASS the uint test.

## Editor Setup (VS Code)

The repository's `.vscode/` settings format every C++ file in Google style when you save. Install this extension to turn it on:

| Extension | ID | What it does |
|---|---|---|
| [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) by Microsoft | `ms-vscode.cpptools` | Formats on save with its built-in clang-format and the repository's `.clang-format` |

VS Code offers to install it when you open the project folder. You can also install it from the terminal:

```
code --install-extension ms-vscode.cpptools
```

No separate clang-format install is needed; the extension includes one.

## Code Style

All C++ code follows the [**Google C++ Style Guide**](https://google.github.io/styleguide/cppguide.html). The cpplint GitHub Action will do the cpplint every push.

- **Name files** in lowercase `snake_case`: source files end in `.cc`, headers in `.h`, and tests in `_test.cc` (`main_menu_screen.cc`, `main_menu_screen.h`, `main_menu_screen_test.cc`).

- **Format** with clang-format. The repository's `.clang-format` selects Google style. VS Code formats on save once the extension above is installed. From the terminal:

  ```
  clang-format -i $(find src test -name '*.h' -o -name '*.cc' -o -name '*.cpp')
  ```

- **Lint** with cpplint before you commit (`brew install cpplint` or `pip install cpplint`):

  ```
  cpplint --recursive src test
  ```

- Do not use any namespace for the code.
- Use `#pragma once` in every header.
- Include what you use: add `#include <string>`, `#include <vector>`, etc. in each file that uses them, even when another header already includes them.

## Team Collaboration

- Use the GitHub Project Kanban to track tasks and progress.
- Avoid modifying code assigned to another teammate without discussing it first.
