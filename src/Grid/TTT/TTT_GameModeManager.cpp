#include "TTT_GameModeManager.hpp"

using namespace TTT;

bool GameBotStart::playTurn() {
    if(fullRun){
        while(!grid.isLocked()){playBot();};
        fullRun = false;
        return true;
    }else if(((startMoveCount > moveHistory.size()) && autoStart)){
        return playBot();
    }else{
        return GameStateBot::GameState::playTurn();
    }

    return false;
}

bool GameBotStart::timeLock(float minTime){
    static auto last_call_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - last_call_time;
    if(elapsed >= std::chrono::duration<double>(minTime)){
        last_call_time = now;
        return false;
    }
    return true;
    
}

bool GameBotStart::playBot() {
    if(fullRun || !timeLock(minBotTime)) {
        return GameStateBot::playBot();
    }
    return false;
}

void GameBotStart::reset(){
    startMoveCount = startMoveDoubt[0];
    if(startMoveDoubt[1] > 0){
        startMoveCount += rand() % (1 + startMoveDoubt[1]);
    }
    autoMode = true;
    GameState::reset();
}

void GameBotStart::showParam(){
    GameStateBot::showParam();
    ImGui::Text("%d,%d,%d", startMoveCount, startMoveDoubt[0], startMoveDoubt[1]);
    ImGui::SliderInt("Count", &startMoveDoubt[0], 0, (int)std::pow(2,7));
    ImGui::SliderInt("Random", &startMoveDoubt[1], 0, (int)std::pow(2,7) -1);
    ImGui::SliderFloat("Wait", &minBotTime, 0, 3);
    ImGui::Checkbox("AutoStart", &autoStart);
    if(fullRun != (fullRun = ImGui::Button("FullRun"))){
        GameBotStart::reset();
    };
}

GameModeManager::GameModeManager(TTT_GridLogic& grid, TTT_GridView& view) {
    // Enregistrement des modes disponibles
    modeFactories["Classic"] = [&]() { return new GameState(grid, view); };
    modeFactories["BotStart"] = [&]() { return new GameBotStart(grid, view); };
    
    // Mode par défaut
    changeGameMode("Classic");
}

void GameModeManager::changeGameMode(const std::string& modeName) {
    if (modeFactories.find(modeName) != modeFactories.end()) {
        currentMode = modeFactories[modeName]();
        currentMode->reset();
        currentModeName = modeName;
    }
}