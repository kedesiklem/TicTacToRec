#pragma once

#include "../Grid/TTT/TTT_GameModeManager.hpp"

#include "emacsStyleShortCut.hpp"

#define SAVE_STATE "save_state.ttt"

namespace TTT {

class CloseWindowFunctor : public Functor {
    GLFWwindow* window;
public:
    CloseWindowFunctor(GLFWwindow* w) : window(w) {}
    void exec() override { glfwSetWindowShouldClose(window, true); }
};

class GameFunctor : public Functor {
protected:
    GameModeManager& gameState;
public:
    GameFunctor(GameModeManager& state) : gameState(state) {}
};

class ResetGameFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override {
        gameState().reset();
    }
};

class UndoMoveFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        gameState().undoLastMove();
    }
};

class RedoMoveFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        gameState().redoLastMove();
    }
};

class PrintValidMovesFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        auto validMove = gameState().grid.getAvailableMoves(gameState().targetSubGridPath);
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
        gameState().saveState(SAVE_STATE);
    }
};

class LoadStateFunctor : public GameFunctor {
public:
    using GameFunctor::GameFunctor;
    void exec() override { 
        gameState().loadState(SAVE_STATE);
    }
};

void setupShortcuts(ShortcutManager& shortcutManager, GLFWwindow* window, GameModeManager& gameMode) {
    
    shortcutManager.addShortcut({{ImGuiKey_Escape}}, 
        new CloseWindowFunctor(window), "Fermer la fenêtre");

    shortcutManager.addShortcut({{ImGuiKey_X, true}, {ImGuiKey_X, true}}, 
        new ResetGameFunctor(gameMode), "Réinitialiser le jeu");

    shortcutManager.addShortcut({{ImGuiKey_Z, true}}, 
        new UndoMoveFunctor(gameMode), "Annuler le dernier mouvement");

    shortcutManager.addShortcut({{ImGuiKey_Y, true}}, 
        new RedoMoveFunctor(gameMode), "Rétablir le dernier mouvement");

    shortcutManager.addShortcut({{ImGuiKey_P}}, 
        new PrintValidMovesFunctor(gameMode), "Afficher les mouvements valides");
    
    shortcutManager.addShortcut({{ImGuiKey_S, true}, {ImGuiKey_S, true}}, 
        new SaveStateFunctor(gameMode), "Sauvegarder l'état");
}

}