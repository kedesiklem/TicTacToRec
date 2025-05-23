cmake_minimum_required(VERSION 3.10)
project(TicTacToRec)

# Configuration des options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuration des répertoires
set(SRC_DIR src)
set(IMGUI_DIR ${SRC_DIR}/external/imgui)
set(IMGUI_FILE_EXPLORER_DIR ${SRC_DIR}/external/imgui_file_explorer)
set(TTT_DIR ${SRC_DIR}/TTT)
set(OTHER_DIR ${SRC_DIR}/utils)

# Configuration des sources principales
set(SOURCES
    ${SRC_DIR}/main.cpp
)

# Sources ImGui (uniquement les fichiers nécessaires)
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
file(GLOB TTT_SOURCES ${TTT_DIR}/*.cpp)

# Sources de l'explorateur de fichiers (si nécessaire)
# file(GLOB IMGUI_FILE_EXPLORER_SOURCES ${IMGUI_FILE_EXPLORER_DIR}/*.cpp)

# Combinaison de toutes les sources
set(ALL_SOURCES
    ${SOURCES}
    ${IMGUI_SOURCES}
    ${TTT_SOURCES}
    # ${IMGUI_FILE_EXPLORER_SOURCES}  # Décommentez si nécessaire
)

# Configuration des include directories
include_directories(
    ${SRC_DIR}
    ${IMGUI_DIR}
    ${IMGUI_FILE_EXPLORER_DIR}
    ${IMGUI_DIR}/backends
    ${TTT_DIR}
    ${OTHER_DIR}
)

# Configuration spécifique à la plateforme
if(UNIX AND NOT APPLE)
    # Linux
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})
    set(LIBS ${GLFW_LIBRARIES} GL)
elseif(APPLE)
    # MacOS
    set(LIBS "-framework OpenGL -framework Cocoa -lglfw")
elseif(WIN32)
    # Windows
    set(LIBS glfw3 gdi32 opengl32 imm32)
endif()

add_compile_options(
    -I${IMGUI_DIR}
    -I${IMGUI_DIR}/backends
)
# Création de l'exécutable
add_executable(tictactorec ${ALL_SOURCES})

# Configuration du répertoire de sortie
set_target_properties(tictactorec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME tictactorec
)

# Liaison des bibliothèques
target_link_libraries(tictactorec ${LIBS})

# Options de compilation
target_compile_options(tictactorec PRIVATE
    -Wall
    -Wformat
    -g
)

# Installation (optionnel)
install(TARGETS tictactorec DESTINATION bin)