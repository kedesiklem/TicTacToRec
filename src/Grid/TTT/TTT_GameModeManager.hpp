#pragma once

#include "TTT_GameStateVariant.hpp"
#include <unistd.h>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <ctime> 

namespace TTT {

class GameBotStart : public GameStateBot {

    bool timeLock(float minTime);

    public:
        bool autoStart = false;
        bool fullRun = false;
        int startMoveCount = 0;
        int startMoveDoubt[2] = {20,5};
        float minBotTime = 0.2;

    using GameStateBot::GameStateBot;

    void showParam() override;
    bool playTurn() override;
    bool playBot();
    void reset() override;
};

class GameModeManager {
    GameState* currentMode;
    std::string currentModeName = "None";
    
    std::map<std::string, std::function<GameState*()>> modeFactories;

public:
    GameModeManager(TTT_GridView& view, TTT_GridLogic& grid);

    GameState& operator()() {
        return *currentMode;
    }

    const std::string& getCurrentModeName() const {
        return currentModeName;
    }

    const std::map<std::string, std::function<GameState*()>>& getAvailableModes() const {
        return modeFactories;
    }

    void changeGameMode(const std::string& modeName);

    bool saveState(const std::string& filename){currentMode->saveState(filename); return true;};
    bool loadState(const std::string& filename){currentMode->loadState(filename); return true;};
};

}