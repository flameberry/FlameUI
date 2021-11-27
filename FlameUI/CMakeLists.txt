file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Actual Library
add_library(FlameUI STATIC ${SRC_FILES})

set_custom_build_properties(FlameUI)

# Defines "FL_DEBUG" for Debug build, and "FL_RELEASE" for Release build
set_build_config_macro_for_target(FlameUI)
target_compile_definitions(FlameUI PRIVATE GLFW_INCLUDE_NONE FL_PROJECT_DIR="${FL_SOURCE_DIR}")

if(${FL_PROJ_GENERATOR} STREQUAL "Xcode")
    target_compile_definitions(FlameUI PRIVATE FL_XCODE_PROJ)
endif()

target_include_directories(FlameUI PRIVATE ${FL_GRAPHICS_INCLUDE_DIRS})

# Add dependencies
add_subdirectory("vendor/GLFW")
add_subdirectory("vendor/Glad")

add_subdirectory("vendor/freetype")
add_library(Freetype::Freetype ALIAS freetype) # Needed for msdfgen find_package(Freetype REQUIRED)

add_subdirectory("vendor/msdfgen")

# "FL_GRAPHICS_LIBS" defined in the {ROOT_DIRECTORY}/CMakeUtils/Dependencies.cmake
target_link_libraries(FlameUI PRIVATE ${FL_GRAPHICS_LIBS})