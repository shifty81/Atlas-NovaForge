cmake_minimum_required(VERSION 3.22)
project(AtlasEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(engine)
add_subdirectory(modules/atlas_gameplay)
add_subdirectory(editor)
add_subdirectory(runtime)
add_subdirectory(client)
add_subdirectory(server)

# Example game modules
add_subdirectory(projects/eveoffline/module)
add_subdirectory(projects/arena2d/module)

add_subdirectory(tileeditor)

enable_testing()
add_subdirectory(tests)
