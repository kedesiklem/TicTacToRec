#pragma once

#include "GameState.hpp"

#include "emacsStyleShortCut.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>

#define SAVE_STATE "save_state.ttt"

class CloseWindowFunctor : public Functor {
    GLFWwindow* window;
public:
    CloseWindowFunctor(GLFWwindow* w) : window(w) {}
    void exec() override { glfwSetWindowShouldClose(window, true); }
};

class GameFunctor : public Functor {
protected:
    GameState& gameState;
public:
    GameFunctor(GameState& state) : gameState(state) {}
};

class ResetGameFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override {
        gameState.reset();
    }
};

class UndoMoveFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        gameState.undoLastMove();
        gameState.undoLastMove();
    }
};

class RedoMoveFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        gameState.redoLastMove();
        gameState.redoLastMove();
    }
};

class PrintValidMovesFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        auto validMove = gameState.grid.grid_root.getAvailableMove(gameState.targetSubGridPath);
        std::cout << "Valid Moves[" << validMove.size() << "]" << std::endl;
        for(auto move : validMove){
            std::cout << "[" << move << "]";
        }
        std::cout << std::endl;
    }
};

class SaveStateFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        gameState.saveState(SAVE_STATE);
    }
};

class LoadStateFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        gameState.loadState(SAVE_STATE);
    }
};

void setupShortcuts(ShortcutManager& shortcutManager, GLFWwindow* window, GameState& gameState) {
    
    shortcutManager.addShortcut({{ImGuiKey_Escape}}, 
        new CloseWindowFunctor(window), "Fermer la fenêtre");

    shortcutManager.addShortcut({{ImGuiKey_X, true}, {ImGuiKey_X, true}}, 
        new ResetGameFunctor(gameState), "Réinitialiser le jeu");

    shortcutManager.addShortcut({{ImGuiKey_Z, true}}, 
        new UndoMoveFunctor(gameState), "Annuler le dernier mouvement");

    shortcutManager.addShortcut({{ImGuiKey_Y, true}}, 
        new RedoMoveFunctor(gameState), "Rétablir le dernier mouvement");

    shortcutManager.addShortcut({{ImGuiKey_P}}, 
        new PrintValidMovesFunctor(gameState), "Afficher les mouvements valides");
    
    shortcutManager.addShortcut({{ImGuiKey_S, true}, {ImGuiKey_S, true}}, 
        new SaveStateFunctor(gameState), "Sauvegarder l'état");

    shortcutManager.addShortcut({{ImGuiKey_S, true, false, true}, {ImGuiKey_S, true, false, true}}, 
        new LoadStateFunctor(gameState), "Charger l'état");
}