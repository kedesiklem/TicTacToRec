#pragma once

#include "TTT_MonteCarlo.hpp"
#include <unistd.h>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <ctime> 

namespace TTT {

class GameRandomStart : public GameState {

    bool timeLock(float minTime);

    public:
        bool autoStart = false;
        bool fullRun = false;
        int startMoveCount = 0;
        int startMoveDoubt[2] = {20,5};
        float minRandomTime = 0.2;

        using GameState::GameState;

        void showParam() override;
        bool playTurn() override;
        bool playRandom();
        void reset() override;
};

class GameModeManager {
    GameState* currentMode;

    
    
    std::map<std::string, std::function<GameState*()>> modeFactories;

public:
    GameModeManager(TTT_GridLogic& grid, TTT_GridView& view);

    GameState& operator()() {
        return *currentMode;
    }

    const std::map<std::string, std::function<GameState*()>>& getAvailableModes() const {
        return modeFactories;
    }

    void changeGameMode(const std::string& modeName);

    bool saveState(const std::string& filename){currentMode->saveState(filename); return true;};
    bool loadState(const std::string& filename){currentMode->loadState(filename); return true;};
};

}