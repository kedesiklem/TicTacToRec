# Configuration des tests
project(TicTacToRecTests LANGUAGES CXX)

# Trouver le framework de test (Google Test recommandé)
find_package(GTest REQUIRED)

# Fichiers sources des tests
file(GLOB TEST_SOURCES *.cpp)

# Création de l'exécutable de test
add_executable(tictactorec_tests ${TEST_SOURCES})

# Liaison avec les bibliothèques nécessaires
target_link_libraries(tictactorec_tests
    PRIVATE
    game_lib
    GTest::GTest
    GTest::Main
)

# Configuration des include directories
target_include_directories(tictactorec_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/UI
)

# Ajout des tests à CTest
add_test(NAME tictactorec_tests COMMAND tictactorec_tests)

# Options de compilation pour les tests
target_compile_options(tictactorec_tests PRIVATE
    -Wall
    -Wformat
    -g
)

# Configuration du répertoire de sortie
set_target_properties(tictactorec_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    OUTPUT_NAME tictactorec_tests
)
