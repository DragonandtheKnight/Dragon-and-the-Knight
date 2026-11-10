cmake_minimum_required(VERSION 3.24)

project(DragonAndTheKnight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(dragon_knight_game STATIC)
target_sources(dragon_knight_game
    PRIVATE
        src/enemy/enemy.cc
        src/enemy/goblin.cc
        src/enemy/slime.cc
        src/game/rng.cc
        src/enemy/shaman.cc
        src/enemy/giant.cc
        src/enemy/dragon.cc
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/game/rng.h
        src/enemy/enemy.h
        src/enemy/enemy_skill.h
        src/enemy/enemy_type.h
        src/enemy/goblin.h
        src/enemy/slime.h
        src/enemy/shaman.h
        src/enemy/giant.h
        src/enemy/dragon.h
)

add_library(card
    src/card/card.cc
    src/card/card.h
    src/card/card_rarity.h
    src/card/card_type.h
    src/card/card_effect.h
    src/card/effect_context.h
)
target_include_directories(card PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(deck src/card/deck.cc src/card/deck.h)
target_link_libraries(deck PUBLIC card dragon_knight_game)

# Game/Player are unfinished. Keep their executable separate so Card tests can
# build independently; explicitly building this target exposes integration gaps.
add_executable(dragon_and_the_knight EXCLUDE_FROM_ALL src/main.cpp)
target_link_libraries(dragon_and_the_knight PRIVATE deck)

enable_testing()

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/googletest EXCLUDE_FROM_ALL)
include(GoogleTest)

add_executable(dragon_knight_game_tests
    test/game/rng_test.cc
    test/enemy/enemy_test.cc
    test/enemy/goblin_test.cc
    test/enemy/slime_test.cc
    test/enemy/shaman_test.cc
    test/enemy/giant_test.cc
    test/enemy/dragon_test.cc
)
target_link_libraries(dragon_knight_game_tests PRIVATE
    dragon_knight_game
    GTest::gtest_main
)
# PRE_TEST lists the tests when ctest runs, so a test binary that crashes on
# start fails the test run instead of the build.
gtest_discover_tests(dragon_knight_game_tests DISCOVERY_MODE PRE_TEST)

add_executable(card_tests test/card/card_test.cc)
target_link_libraries(card_tests PRIVATE card GTest::gtest_main)
gtest_discover_tests(card_tests TEST_PREFIX "card." DISCOVERY_MODE PRE_TEST)

add_executable(deck_tests test/card/deck_test.cc)
target_link_libraries(deck_tests PRIVATE deck GTest::gtest_main)
gtest_discover_tests(deck_tests TEST_PREFIX "deck." DISCOVERY_MODE PRE_TEST)
