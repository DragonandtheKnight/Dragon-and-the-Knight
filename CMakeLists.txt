cmake_minimum_required(VERSION 3.24)

project(DragonAndTheKnight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(FTXUI_QUIET ON CACHE BOOL "" FORCE)
add_subdirectory(lib/ftxui EXCLUDE_FROM_ALL)

# The UI is header-only until its screens are implemented. Verifying the header
# set compiles every header on its own, so a broken declaration fails the build.
add_library(dragon_knight_ui INTERFACE)
target_sources(dragon_knight_ui INTERFACE
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/game/difficulty.h
        src/ui/app.h
        src/ui/battle_screen.h
        src/ui/exchange_card_screen.h
        src/ui/game_over_screen.h
        src/ui/inventory_screen.h
        src/ui/loot_screen.h
        src/ui/main_menu_screen.h
        src/ui/map_screen.h
        src/ui/settings_screen.h
        src/ui/shop_screen.h
        src/ui/widgets.h
)
target_link_libraries(dragon_knight_ui INTERFACE
    ftxui::component
    ftxui::dom
    ftxui::screen
)
set_target_properties(dragon_knight_ui PROPERTIES
    VERIFY_INTERFACE_HEADER_SETS ON
)
add_custom_target(check_ui_headers ALL)
add_dependencies(check_ui_headers dragon_knight_ui_verify_interface_header_sets)

enable_testing()

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/googletest EXCLUDE_FROM_ALL)
include(GoogleTest)

add_executable(dragon_knight_tests
    test/ui/main_menu_screen_test.cc
)
target_link_libraries(dragon_knight_tests PRIVATE
    dragon_knight_ui
    GTest::gtest_main
)
# PRE_TEST lists the tests when ctest runs, so a test binary that crashes on
# start fails the test run instead of the build.
gtest_discover_tests(dragon_knight_tests DISCOVERY_MODE PRE_TEST)
