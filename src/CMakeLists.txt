# Configuration des répertoires
set(IMGUI_DIR external/imgui)
set(IMGUI_FILE_EXPLORER_DIR external/imgui_file_explorer)
set(GRID_DIR Grid)
set(OTHER_DIR utils)

# Sources ImGui
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Sources du jeu
file(GLOB GRID_SOURCES ${GRID_DIR}/*.cpp)
file(GLOB TTT_SOURCES ${GRID_DIR}/Rune/*.cpp)

# Création de la bibliothèque principale
add_library(game_lib STATIC
    ${GRID_SOURCES}
    ${TTT_SOURCES}
    ${IMGUI_SOURCES}
)

# Configuration des include directories
target_include_directories(game_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_FILE_EXPLORER_DIR}
    ${IMGUI_DIR}/backends
    ${GRID_DIR}
    ${OTHER_DIR}
)

# Find GLFW package
find_package(glfw3 3.3 REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Configuration spécifique à la plateforme
if(UNIX AND NOT APPLE)
    target_link_libraries(game_lib PRIVATE glfw OpenGL::OpenGL OpenGL::GLX)
elseif(APPLE)
    target_link_libraries(game_lib PRIVATE glfw OpenGL::OpenGL "-framework Cocoa -framework IOKit")
elseif(WIN32)
    target_link_libraries(game_lib PRIVATE glfw opengl32)
endif()

# Création de l'exécutable principal
add_executable(runebuilder main.cpp)
target_link_libraries(runebuilder PRIVATE game_lib)

# Options de compilation
target_compile_options(runebuilder PRIVATE
    -Wall
    -Wformat
    -g
)

# Configuration du répertoire de sortie
set_target_properties(runebuilder PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME runebuilder
)

# Installation (optionnel)
install(TARGETS runebuilder DESTINATION bin)