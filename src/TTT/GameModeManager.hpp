#pragma once

#include "GameState.hpp"
#include <unistd.h>
#include <memory>
#include <map>
#include <functional>

class GameRandomStart : public GameState {
    int startMoveCount = INFINITY;
    double minRandomTime = 0; // 1s = 1e6

public:
    GameRandomStart(GridView& grid) : GameState(grid) {}

    bool playTurn() override;
    bool playRandom();
};

class GameModeManager {
    GameState* currentMode;
    
    std::map<std::string, std::function<GameState*()>> modeFactories;

public:
    GameModeManager(GridView& grid) {
        // Enregistrement des modes disponibles
        modeFactories["Classic"] = [&]() { return new GameState(grid); };
        modeFactories["RandomStart"] = [&]() { return new GameRandomStart(grid); };
        
        // Mode par défaut
        changeGameMode("Classic");
    }

    GameState& operator()() {
        return *currentMode;
    }

    const std::map<std::string, std::function<GameState*()>>& getAvailableModes() const {
        return modeFactories;
    }

    void changeGameMode(const std::string& modeName) {
        if (modeFactories.find(modeName) != modeFactories.end()) {
            currentMode = modeFactories[modeName]();
            currentMode->reset();
        }
    }
};