#pragma once

#include "GameState.hpp"
#include <unistd.h>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <ctime> 

class GameRandomStart : public GameState {

    bool timeLock(float minTime);

    public:
        bool autoStart = false;
        bool fullRun = false;
        int startMoveCount = 0;
        int startMoveDoubt[2] = {20,5};
        float minRandomTime = 0.2;

    GameRandomStart(GridView& grid) : GameState(grid) {}

    void showParam() override;
    bool playTurn() override;
    bool playRandom();
    void reset() override;
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