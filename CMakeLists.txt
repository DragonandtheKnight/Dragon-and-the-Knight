cmake_minimum_required(VERSION 3.24)

project(DragonAndTheKnight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(FTXUI_QUIET ON CACHE BOOL "" FORCE)
add_subdirectory(lib/ftxui EXCLUDE_FROM_ALL)

add_library(dragon_knight_game STATIC)
target_sources(dragon_knight_game
    PRIVATE
        src/card/deck.cc
        src/game/game_state_machine.cc
        src/game/rng.cc
        src/player/player.cc
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/card/deck.h
        src/game/difficulty.h
        src/game/game_state_machine.h
        src/game/rng.h
        src/player/player.h
)

# Screens whose headers are listed but whose .cc is not yet here wait for game
# classes (Card, FloorMap, Battle, Shop). Verifying the header set still
# compiles every header on its own, so a broken declaration fails the build.
add_library(dragon_knight_ui STATIC)
target_sources(dragon_knight_ui
    PRIVATE
        src/ui/game_over_screen.cc
        src/ui/main_menu_screen.cc
        src/ui/settings_screen.cc
        src/ui/widgets.cc
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
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
target_link_libraries(dragon_knight_ui PUBLIC
    dragon_knight_game
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

add_executable(dragon_knight_game_tests
    test/game/game_state_machine_test.cc
    test/game/rng_test.cc
    test/player/player_test.cc
)
target_link_libraries(dragon_knight_game_tests PRIVATE
    dragon_knight_game
    GTest::gtest_main
)

add_executable(dragon_knight_tests
    test/ui/game_over_screen_test.cc
    test/ui/main_menu_screen_test.cc
    test/ui/settings_screen_test.cc
    test/ui/widgets_test.cc
)
target_include_directories(dragon_knight_tests PRIVATE test)
target_link_libraries(dragon_knight_tests PRIVATE
    dragon_knight_ui
    GTest::gtest_main
)

# PRE_TEST lists the tests when ctest runs, so a test binary that crashes on
# start fails the test run instead of the build.
gtest_discover_tests(dragon_knight_game_tests DISCOVERY_MODE PRE_TEST)
gtest_discover_tests(dragon_knight_tests DISCOVERY_MODE PRE_TEST)
