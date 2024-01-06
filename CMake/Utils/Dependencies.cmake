# OpenGL Helper Libs
set(FL_GRAPHICS_LIBS glfw Glad freetype msdfgen-core msdfgen-ext)

if(APPLE)
    # Inbuilt mac frameworks required for GLFW
    list(APPEND FL_GRAPHICS_LIBS 
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreFoundation"
    )
elseif(WIN32)
    list(APPEND FL_GRAPHICS_LIBS opengl32.lib)
endif()

# All Include Dirs needed for the project
set(FL_GRAPHICS_INCLUDE_DIRS
    ${FL_SOURCE_DIR}/FlameUI/src
    ${FL_SOURCE_DIR}/FlameUI/vendor
    ${FL_SOURCE_DIR}/FlameUI/vendor/GLFW/include
    ${FL_SOURCE_DIR}/FlameUI/vendor/Glad/include
    ${FL_SOURCE_DIR}/FlameUI/vendor/glm
    ${FL_SOURCE_DIR}/FlameUI/vendor/freetype/include
)
