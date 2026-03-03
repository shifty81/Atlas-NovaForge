# Atlas - Root CMakeLists.txt
# Unified build: Atlas Engine core + Nova Forge game project (client & server)
#
# Architecture:
#   engine/             → AtlasEngine static library (generic, game-agnostic)
#   modules/            → Engine modules (atlas_gameplay, etc.)
#   editor/             → AtlasEditor executable (authoring tool)
#   runtime/            → Atlas runtime
#   client/             → Atlas client
#   server/             → Atlas server
#   cpp_client/         → Nova Forge game client (links AtlasEngine)
#   cpp_server/         → Nova Forge game server (links AtlasEngine)
#   projects/           → Example game modules
#   tileeditor/         → Tile editor tool
#   tests/              → Legacy tests
#   atlas_tests/        → Atlas engine unit tests

cmake_minimum_required(VERSION 3.22)
project(AtlasEngine VERSION 1.0.0 LANGUAGES C CXX)

# C++20 standard (Atlas-NovaForge requirement)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Unified output directories so all executables land together
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(BUILD_CLIENT "Build C++ OpenGL client" ON)
option(BUILD_SERVER "Build C++ dedicated server" ON)
option(BUILD_ATLAS_ENGINE "Build Atlas Engine library" ON)
option(BUILD_ATLAS_EDITOR "Build Atlas Editor" ON)
option(BUILD_ATLAS_TESTS "Build Atlas Engine tests" ON)

# --- Atlas Engine (core library) ---
if(BUILD_ATLAS_ENGINE)
    message(STATUS "Adding Atlas Engine library...")
    add_subdirectory(engine)
endif()

# --- Engine Modules ---
if(BUILD_ATLAS_ENGINE)
    add_subdirectory(modules/atlas_gameplay)
endif()

# --- Atlas Editor ---
if(BUILD_ATLAS_EDITOR AND BUILD_ATLAS_ENGINE)
    message(STATUS "Adding Atlas Editor...")
    add_subdirectory(editor)
endif()

# --- Atlas Runtime ---
add_subdirectory(runtime)

# --- Atlas Client & Server ---
add_subdirectory(client)
add_subdirectory(server)

# --- Example game modules ---
add_subdirectory(projects/eveoffline/module)
add_subdirectory(projects/arena2d/module)
add_subdirectory(projects/novaforge/module)

# --- Tile Editor ---
add_subdirectory(tileeditor)

# --- Atlas Engine Tests ---
if(BUILD_ATLAS_TESTS AND BUILD_ATLAS_ENGINE)
    enable_testing()
    message(STATUS "Adding Atlas Engine tests...")
    add_subdirectory(atlas_tests)
endif()

# --- Legacy Tests ---
enable_testing()
add_subdirectory(tests)

# --- Nova Forge Game Client ---
if(BUILD_CLIENT)
    message(STATUS "Adding C++ Client to solution...")
    add_subdirectory(cpp_client)
endif()

# --- Nova Forge Game Server ---
if(BUILD_SERVER)
    message(STATUS "Adding C++ Server to solution...")
    add_subdirectory(cpp_server)
endif()

message(STATUS "")
message(STATUS "Atlas Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Atlas Engine:  ${BUILD_ATLAS_ENGINE}")
message(STATUS "  Atlas Editor:  ${BUILD_ATLAS_EDITOR}")
message(STATUS "  Atlas Tests:   ${BUILD_ATLAS_TESTS}")
message(STATUS "  Game Client:   ${BUILD_CLIENT}")
message(STATUS "  Game Server:   ${BUILD_SERVER}")
message(STATUS "")
message(STATUS "After generation, open the solution file in Visual Studio:")
message(STATUS "  build_vs/Atlas.sln")
message(STATUS "")
