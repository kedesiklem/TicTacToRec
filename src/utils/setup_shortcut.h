#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>

#include <emacsStyleShortCut.hpp>
#include <GameState.h>

// D'abord, créons les classes Functor pour chaque action

class CloseWindowFunctor : public Functor {
    GLFWwindow* window;
public:
    CloseWindowFunctor(GLFWwindow* w) : window(w) {}
    void exec() override { glfwSetWindowShouldClose(window, true); }
};

class ResetGameFunctor : public Functor {
    Grid& mainGrid;
    GameState& gameState;
public:
    ResetGameFunctor(Grid& grid, GameState& state) : mainGrid(grid), gameState(state) {}
    void exec() override { 
        mainGrid.resetGrid();
        gameState.reset();
    }
};

class UndoMoveFunctor : public Functor {
    Grid& mainGrid;
    GameState& gameState;
public:
    UndoMoveFunctor(Grid& grid, GameState& state) : mainGrid(grid), gameState(state) {}
    void exec() override { 
        gameState.undoLastMove(mainGrid);
    }
};

class RedoMoveFunctor : public Functor {
    Grid& mainGrid;
    GameState& gameState;
public:
    RedoMoveFunctor(Grid& grid, GameState& state) : mainGrid(grid), gameState(state) {}
    void exec() override { 
        gameState.redoLastMove(mainGrid);
    }
};

class PrintValidMovesFunctor : public Functor {
    Grid& mainGrid;
    GameState& gameState;
public:
    PrintValidMovesFunctor(Grid& grid, GameState& state) : mainGrid(grid), gameState(state) {}
    void exec() override { 
        std::cout << "Valid Moves" << std::endl;
        auto validMove = mainGrid.getValidMoves(gameState.targetSubGridPath);
        for(auto move : validMove){
            for(size_t i=0; i<move.size() - 1; ++i){
                std::cout << move[i] << ",";
            }
            if(move.size() > 0){
                std::cout << move[move.size() - 1];
            }
            std::cout << std::endl;
        }
    }
};


void setupShortcuts(ShortcutManager& shortcutManager, GLFWwindow* window, Grid& mainGrid, GameState& gameState) {
    // Escape pour fermer la fenêtre
    shortcutManager.addShortcut({{ImGuiKey_Escape}}, 
        new CloseWindowFunctor(window), "Fermer la fenêtre");

    // Ctrl-X Ctrl-X pour réinitialiser le jeu
    shortcutManager.addShortcut({{ImGuiKey_X, true}, {ImGuiKey_X, true}}, 
        new ResetGameFunctor(mainGrid, gameState), "Réinitialiser le jeu");

    // Ctrl-Z pour annuler le dernier mouvement
    shortcutManager.addShortcut({{ImGuiKey_Z, true}}, 
        new UndoMoveFunctor(mainGrid, gameState), "Annuler le dernier mouvement");

    // Ctrl-Y pour rétablir le dernier mouvement
    shortcutManager.addShortcut({{ImGuiKey_Y, true}}, 
        new RedoMoveFunctor(mainGrid, gameState), "Rétablir le dernier mouvement");

    // P pour afficher les mouvements valides
    shortcutManager.addShortcut({{ImGuiKey_P}}, 
        new PrintValidMovesFunctor(mainGrid, gameState), "Afficher les mouvements valides");
}