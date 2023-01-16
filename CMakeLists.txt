cmake_minimum_required(VERSION 3.16)

# Sets project name
project(volumetric-clouds LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    # Source files
    src/main.cpp
    src/realtime.cpp
    src/mainwindow.cpp
    src/settings.cpp

    src/camera/camera.cpp
    src/noise/worley.cpp
    src/noise/perlin.cpp
    src/noise/perlin-zhou.cpp

#    src/terrain/glwidget.cpp
    src/terrain/terraingenerator.cpp

    src/glStructure/FBO.cpp


    # Header files
    src/mainwindow.h
    src/realtime.h
    src/settings.h

    src/utils/shaderloader.h

#    src/terrain/glwidget.h
    src/terrain/terraingenerator.h

    src/camera/camera.h
    src/noise/worley.h
    src/noise/perlin.h
    src/noise/perlin-zhou.h

    src/glStructure/FBO.h

    src/utils/debug.h

    src/mainwindow.ui
)

# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Xml
    StaticGLEW
)

# Specifies other files
qt6_add_resources(${PROJECT_NAME} "Resources"
    PREFIX
        "/"
    FILES
        resources/shaders/default.frag
        resources/shaders/default.vert
        resources/shaders/worley.comp

        resources/shaders/terrain_generator.vert
        resources/shaders/terrain_generator.frag
        resources/shaders/terrain_texture.vert
        resources/shaders/terrain_texture.frag

)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
