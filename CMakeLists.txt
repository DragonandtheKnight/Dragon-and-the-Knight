
cmake_minimum_required(VERSION 3.24)

project(DragonAndTheKnight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(FTXUI_QUIET ON CACHE BOOL "" FORCE)
add_subdirectory(lib/ftxui EXCLUDE_FROM_ALL)

# Core game and enemy implementations
add_library(dragon_knight_game STATIC)
target_sources(dragon_knight_game
    PRIVATE
        src/game/game_state_machine.cc
        src/game/rng.cc
        src/enemy/enemy.cc
        src/enemy/goblin.cc
        src/enemy/slime.cc
        src/enemy/shaman.cc
        src/enemy/giant.cc
        src/enemy/dragon.cc
        src/enemy/empty_enemy.cc
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/game/difficulty.h
        src/game/game_state_machine.h
        src/game/rng.h
        src/enemy/enemy.h
        src/enemy/enemy_skill.h
        src/enemy/enemy_type.h
        src/enemy/goblin.h
        src/enemy/slime.h
        src/enemy/shaman.h
        src/enemy/giant.h
        src/enemy/dragon.h
        src/enemy/empty_enemy.h
)

# Card library
add_library(card
    src/card/card.cc
    src/card/card.h
    src/card/card_rarity.h
    src/card/card_type.h
    src/card/card_effect.h
    src/card/effect_context.h
)
target_include_directories(card PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Deck library (from main / PR #41)
add_library(deck src/card/deck.cc src/card/deck.h)
target_link_libraries(deck PUBLIC card dragon_knight_game)

# Player library (from PR #21)
add_library(player STATIC src/player/player.cc)
target_sources(player
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/player/player.h
)
target_link_libraries(player PUBLIC deck)

# Level and map library
add_library(level_map STATIC
    src/level/level.cc
    src/level/level.h
    src/level/level_type.h

    src/level/shop.cc
    src/level/shop.h
    src/level/rest.cc
    src/level/rest.h
    src/level/event.cc
    src/level/event.h
    src/level/entertainment.cc
    src/level/entertainment.h

    src/map/floor_map.cc
    src/map/floor_map.h
)

target_include_directories(level_map PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(level_map PUBLIC player dragon_knight_game)

# Battle library
add_library(combat STATIC
    src/combat/combat.cc
    src/combat/combat.h
    src/combat/skill_cards/damage_player_effect.cc
    src/combat/skill_cards/damage_player_effect.h
    src/combat/skill_cards/heal_target_effect.cc
    src/combat/skill_cards/heal_target_effect.h
    src/combat/skill_cards/shield_target_effect.cc
    src/combat/skill_cards/shield_target_effect.h
    src/combat/skill_cards/heal_enemies_effect.cc
    src/combat/skill_cards/heal_enemies_effect.h
    src/combat/skill_cards/shield_max_hp_enemy_effect.cc
    src/combat/skill_cards/shield_max_hp_enemy_effect.h
)
target_include_directories(combat PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(combat PUBLIC player buff)

# Buff library
add_library(buff STATIC src/combat/buff.cc src/combat/buff.h)
target_include_directories(buff PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(buff PUBLIC card)

# Screens whose headers are listed but whose .cc is not yet here wait for game
# classes (Card, FloorMap, Battle, Shop). Verifying the header set still
# compiles every header on its own, so a broken declaration fails the build.
add_library(dragon_knight_ui STATIC)
target_sources(dragon_knight_ui
    PRIVATE
        src/ui/app.cc
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
    level_map
    ftxui::component
    ftxui::dom
    ftxui::screen
)
set_target_properties(dragon_knight_ui PROPERTIES
    VERIFY_INTERFACE_HEADER_SETS ON
)
add_custom_target(check_ui_headers ALL)
add_dependencies(check_ui_headers dragon_knight_ui_verify_interface_header_sets)

# The game
add_executable(dragon_and_the_knight src/main.cc)
target_link_libraries(dragon_and_the_knight PRIVATE dragon_knight_ui)

enable_testing()

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/googletest EXCLUDE_FROM_ALL)
include(GoogleTest)

# PRE_TEST lists the tests when ctest runs, so a test binary that crashes on
# start fails the test run instead of the build.

# Core game and enemy tests
add_executable(dragon_knight_game_tests
    test/game/game_state_machine_test.cc
    test/game/rng_test.cc
    test/enemy/enemy_test.cc
    test/enemy/goblin_test.cc
    test/enemy/slime_test.cc
    test/enemy/shaman_test.cc
    test/enemy/giant_test.cc
    test/enemy/dragon_test.cc
    test/enemy/empty_enemy_test.cc
)
target_link_libraries(dragon_knight_game_tests PRIVATE
    dragon_knight_game
    GTest::gtest_main
)

gtest_discover_tests(
    dragon_knight_game_tests
    DISCOVERY_MODE PRE_TEST
)

# Player tests
add_executable(player_tests test/player/player_test.cc)
target_link_libraries(player_tests PRIVATE
    player
    GTest::gtest_main
)
gtest_discover_tests(
    player_tests
    TEST_PREFIX "player."
    DISCOVERY_MODE PRE_TEST
)

# Card tests
add_executable(card_tests test/card/card_test.cc)
target_link_libraries(card_tests PRIVATE card GTest::gtest_main)
gtest_discover_tests(card_tests TEST_PREFIX "card." DISCOVERY_MODE PRE_TEST)

# Deck tests
add_executable(deck_tests test/card/deck_test.cc)
target_link_libraries(deck_tests PRIVATE deck GTest::gtest_main)
gtest_discover_tests(deck_tests TEST_PREFIX "deck." DISCOVERY_MODE PRE_TEST)

# Level and map tests
add_executable(level_map_tests test/map/floor_map_test.cc)
target_link_libraries(level_map_tests PRIVATE level_map GTest::gtest_main)
gtest_discover_tests(level_map_tests DISCOVERY_MODE PRE_TEST)

# Non-combat level tests
add_executable(non_combat_level_tests
    test/level/shop_test.cc
    test/level/rest_test.cc
    test/level/event_test.cc
    test/level/entertainment_test.cc
)

target_link_libraries(non_combat_level_tests PRIVATE
    level_map
    player
    GTest::gtest_main
)

gtest_discover_tests(
    non_combat_level_tests
    TEST_PREFIX "non_combat."
    DISCOVERY_MODE PRE_TEST
)

# Battle tests
add_executable(combat_tests
    test/combat/combat_test.cc
    test/combat/combat_buff_test.cc
    test/combat/damage_player_effect_test.cc
    test/combat/heal_target_effect_test.cc
    test/combat/shield_target_effect_test.cc
    test/combat/heal_enemies_effect_test.cc
    test/combat/shield_max_hp_enemy_effect_test.cc
)
target_link_libraries(combat_tests PRIVATE combat GTest::gtest_main)
gtest_discover_tests(combat_tests TEST_PREFIX "combat." DISCOVERY_MODE PRE_TEST)

# Buff tests
add_executable(buff_tests test/buff/buff_test.cc test/buff/buff_apply_test.cc)
target_link_libraries(buff_tests PRIVATE buff GTest::gtest_main)
gtest_discover_tests(
    buff_tests
    TEST_PREFIX "buff."
    DISCOVERY_MODE PRE_TEST
)

# UI tests
add_executable(dragon_knight_tests
    test/ui/app_test.cc
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
gtest_discover_tests(dragon_knight_tests DISCOVERY_MODE PRE_TEST)
