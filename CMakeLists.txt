
cmake_minimum_required(VERSION 3.24)

project(DragonAndTheKnight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==========================================
# Core game and enemy implementations
# ==========================================

add_library(dragon_knight_game STATIC)

target_sources(dragon_knight_game
    PRIVATE
        src/game/rng.cc
        src/enemy/enemy.cc
        src/enemy/goblin.cc
        src/enemy/slime.cc
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

# ==========================================
# Card library
# ==========================================

add_library(card
    src/card/card.cc
    src/card/card.h
    src/card/card_rarity.h
    src/card/card_type.h
    src/card/card_effect.h
    src/card/effect_context.h
)

target_include_directories(card PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ==========================================
# Deck library
# ==========================================

add_library(deck
    src/card/deck.cc
    src/card/deck.h
)

target_link_libraries(deck PUBLIC
    card
    dragon_knight_game
)

# ==========================================
# Player library
# ==========================================

add_library(player STATIC
    src/player/player.cc
)

target_sources(player
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/player/player.h
)

target_link_libraries(player PUBLIC deck)

# ==========================================
# Level and map library
# ==========================================

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

target_include_directories(level_map PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(level_map PUBLIC player)

# ==========================================
# Main executable
# ==========================================

# Game/Player integration is still unfinished.
add_executable(dragon_and_the_knight
    EXCLUDE_FROM_ALL
    src/main.cpp
)

target_link_libraries(dragon_and_the_knight PRIVATE
    player
    level_map
)

# ==========================================
# GoogleTest configuration
# ==========================================

enable_testing()

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/googletest EXCLUDE_FROM_ALL)

include(GoogleTest)

# ==========================================
# Core game and enemy tests
# ==========================================

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

gtest_discover_tests(
    dragon_knight_game_tests
    DISCOVERY_MODE PRE_TEST
)

# ==========================================
# Player tests
# ==========================================

add_executable(player_tests
    test/player/player_test.cc
)

target_link_libraries(player_tests PRIVATE
    player
    GTest::gtest_main
)

gtest_discover_tests(
    player_tests
    TEST_PREFIX "player."
    DISCOVERY_MODE PRE_TEST
)

# ==========================================
# Card tests
# ==========================================

add_executable(card_tests
    test/card/card_test.cc
)

target_link_libraries(card_tests PRIVATE
    card
    GTest::gtest_main
)

gtest_discover_tests(
    card_tests
    TEST_PREFIX "card."
    DISCOVERY_MODE PRE_TEST
)

# CardEffect TDD: preserve the original 195 regression tests.
# Example: cmake -S . -B <build-dir> -DCARD_TDD_POISON=ON
#          cmake --build <build-dir> --target card_tdd_poison
# Future modules deliberately remain compile-time RED until proposed APIs exist.
function(add_card_tdd module enabled)
    string(TOUPPER "${module}" upper)
    option(CARD_TDD_${upper} "Build ${module} tests; read source contract first" ${enabled})
    if(CARD_TDD_${upper})
        add_executable(card_tdd_${module} ${ARGN})
        target_link_libraries(card_tdd_${module} PRIVATE player GTest::gtest_main)
        gtest_discover_tests(card_tdd_${module} TEST_PREFIX "card_tdd.${module}."
            DISCOVERY_MODE PRE_TEST PROPERTIES LABELS "card_tdd;${module}")
    else()
        message(STATUS "Card TDD ${module}: OFF (not compiled/executed)")
    endif()
endfunction()
# All five existing test files are retained verbatim. Keep basic implementations
# in card, preserving their public library linkage. Each option gates both the
# existing source and its tests (use OFF for an unfinished implementation).
function(add_basic_card_tdd module)
    string(TOUPPER "${module}" upper)
    option(CARD_TDD_${upper} "Build existing ${module} implementation and tests" ON)
    if(CARD_TDD_${upper})
        target_sources(card PRIVATE src/card/${module}_effect.cc)
    endif()
    add_card_tdd(${module} ON test/card/${module}_effect_test.cc)
endfunction()
add_basic_card_tdd(damage)
add_basic_card_tdd(block)
add_basic_card_tdd(heal)
# Planned API modules, independently opt-in; not claims of passing tests.
add_card_tdd(target OFF test/card/effect_target_test.cc)
add_card_tdd(multi OFF test/card/card_multi_effect_test.cc)
add_card_tdd(poison OFF test/card/poison_effect_test.cc)
add_card_tdd(bleeding OFF test/card/bleeding_effect_test.cc)
add_card_tdd(hot OFF test/card/hot_effect_test.cc)
add_card_tdd(buff OFF test/card/buff_test.cc)
add_card_tdd(calculation OFF test/card/buff_calculation_test.cc)
add_card_tdd(status OFF test/card/status_manager_test.cc)
add_card_tdd(factory OFF test/card/enemy_card_factory_test.cc)

# ==========================================
# Deck tests
# ==========================================

add_executable(deck_tests
    test/card/deck_test.cc
)

target_link_libraries(deck_tests PRIVATE
    deck
    GTest::gtest_main
)

gtest_discover_tests(
    deck_tests
    TEST_PREFIX "deck."
    DISCOVERY_MODE PRE_TEST
)

# ==========================================
# Level and map tests
# ==========================================

add_executable(level_map_tests
    test/map/floor_map_test.cc
)

target_link_libraries(level_map_tests PRIVATE
    level_map
    GTest::gtest_main
)

gtest_discover_tests(
    level_map_tests
    DISCOVERY_MODE PRE_TEST
)

# ==========================================
# Non-combat level tests
# ==========================================

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
