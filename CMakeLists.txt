cmake_minimum_required(VERSION 3.24)

project(DragonAndTheKnight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(dragon_knight_game STATIC)
target_sources(dragon_knight_game
    PRIVATE
        src/enemy/Enemy.cc
        src/enemy/Goblin.cc
        src/enemy/Slime.cc
        src/game/rng.cc
        src/enemy/Shaman.cc
        src/enemy/Giant.cc
        src/enemy/Dragon.cc
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/game/rng.h
        src/enemy/Enemy.h
        src/enemy/Enemy_skill.h
        src/enemy/Enemy_type.h
        src/enemy/Goblin.h
        src/enemy/Slime.h
        src/enemy/Shaman.h
        src/enemy/Giant.h
        src/enemy/Dragon.h
)

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
