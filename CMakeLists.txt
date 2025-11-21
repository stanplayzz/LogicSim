cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# sfml
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)

FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(main ${SOURCES})
target_compile_features(main PRIVATE cxx_std_23)
target_compile_definitions(main PRIVATE ASSETS_DIR="${PROJECT_SOURCE_DIR}/assets")
target_link_libraries(main PRIVATE SFML::Graphics)
target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/src)