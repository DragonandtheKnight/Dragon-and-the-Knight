cmake_minimum_required(VERSION 3.24)

project(DragonAndTheKnight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(card
    src/card/Card.cc
    src/card/Card.h
    src/card/CardRarity.h
    src/card/CardType.h
    src/card/card_effect.h
    src/card/effect_context.h
)
target_include_directories(card PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(rng src/game/rng.cc src/game/rng.h)
target_include_directories(rng PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(deck src/card/deck.cc src/card/deck.h)
target_link_libraries(deck PUBLIC card rng)

# Game/Player are unfinished. Keep their executable separate so Card tests can
# build independently; explicitly building this target exposes integration gaps.
add_executable(dragon_and_the_knight EXCLUDE_FROM_ALL src/main.cpp)
target_link_libraries(dragon_and_the_knight PRIVATE deck)

include(CTest)
if(BUILD_TESTING)
    # Use the repository's GoogleTest dependency. An external source path allows
    # testing the same pinned revision without changing an empty submodule.
    set(DRAGON_GOOGLETEST_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest"
        CACHE PATH "Path to the repository's GoogleTest source dependency")
    if(NOT EXISTS "${DRAGON_GOOGLETEST_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR
            "GoogleTest sources are missing from ${DRAGON_GOOGLETEST_SOURCE_DIR}. "
            "Populate the registered lib/googletest submodule, or set "
            "DRAGON_GOOGLETEST_SOURCE_DIR to its pinned source revision. "
            "Use BUILD_TESTING=OFF to build only the libraries.")
    endif()
    add_subdirectory("${DRAGON_GOOGLETEST_SOURCE_DIR}"
                     "${CMAKE_CURRENT_BINARY_DIR}/googletest" EXCLUDE_FROM_ALL)
    include(GoogleTest)

    add_executable(card_tests test/card/card_test.cc)
    target_link_libraries(card_tests PRIVATE card GTest::gtest_main)
    gtest_discover_tests(card_tests TEST_PREFIX "card.")

    # Intentionally included in the default build: the missing Deck definitions
    # must remain visible as TDD Red. Build card_tests explicitly to check Card.
    add_executable(deck_tests test/card/deck_test.cc)
    target_link_libraries(deck_tests PRIVATE deck GTest::gtest_main)
    gtest_discover_tests(deck_tests TEST_PREFIX "deck.")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(dragon_knight_game STATIC)
target_sources(dragon_knight_game
    PRIVATE
        src/game/rng.cc
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/game/rng.h
)

enable_testing()

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/googletest EXCLUDE_FROM_ALL)
include(GoogleTest)

add_executable(dragon_knight_game_tests
    test/game/rng_test.cc
)
target_link_libraries(dragon_knight_game_tests PRIVATE
    dragon_knight_game
    GTest::gtest_main
)
# PRE_TEST lists the tests when ctest runs, so a test binary that crashes on
# start fails the test run instead of the build.
gtest_discover_tests(dragon_knight_game_tests DISCOVERY_MODE PRE_TEST)
