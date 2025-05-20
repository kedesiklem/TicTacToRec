#pragma once

#include "GameState.hpp"
#include <unistd.h>
#include <memory>
#include <map>
#include <functional>

class GameRandomStart : public GameState {
    public:
        bool autoStart = true;
        bool fullRun = false;
        int startMoveCount = 0;
        int startMoveDoubt[2] = {25,5};
        float minRandomTime = 2e5; // 1s = 1e6

public:
    GameRandomStart(GridView& grid) : GameState(grid) {}

    void showParam() override;
    bool playTurn() override;
    bool playRandom();
    void reset();
};

class GameModeManager {
    GameState* currentMode;
    
    std::map<std::string, std::function<GameState*()>> modeFactories;

public:
    GameModeManager(GridView& grid);

    GameState& operator()() {
        return *currentMode;
    }

    const std::map<std::string, std::function<GameState*()>>& getAvailableModes() const {
        return modeFactories;
    }

    void changeGameMode(const std::string& modeName);
};