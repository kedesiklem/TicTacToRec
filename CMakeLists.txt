cmake_minimum_required(VERSION 3.10)
project(TicTacToRec VERSION 1.0 LANGUAGES CXX)

# Set policy to prefer GLVND (new OpenGL library structure)
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# Configuration des options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option pour activer/désactiver les tests
option(BUILD_TESTS "Build tests" OFF)

# Configuration des sous-répertoires
add_subdirectory(src)

# Ajout des tests si l'option est activée
message(STATUS "BUILD_TESTS = ${BUILD_TESTS}")
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()