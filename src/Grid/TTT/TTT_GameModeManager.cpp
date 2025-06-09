#include "TTT_GameModeManager.hpp"

using namespace TTT;

bool GameRandomStart::playTurn() {
    if(fullRun){
        while(!grid.isLocked()){playRandom();};
        fullRun = false;
        return true;
    }else if((isBotPlayer(currentPlayer)) || ((startMoveCount > moveHistory.size()) && autoStart)){
        return playRandom();
    }else{
        auto path = view.handleGridInteraction(grid);
        if(path){
            return playMove(path.value());
        }
    }

    return false;
}

bool GameRandomStart::timeLock(float minTime){
    static auto last_call_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - last_call_time;
    if(elapsed >= std::chrono::duration<double>(minTime)){
        last_call_time = now;
        return false;
    }
    return true;
    
}

bool GameRandomStart::playRandom() {


    if(fullRun || !timeLock(minRandomTime)) {
        return GameState::playRandom();
    }
    return false;
}

void GameRandomStart::reset(){
    startMoveCount = startMoveDoubt[0];
    if(startMoveDoubt[1] > 0){
        startMoveCount += rand() % (1 + startMoveDoubt[1]);
    }
    GameState::reset();
}

void GameRandomStart::showParam(){
    ImGui::Text("%d,%d,%d", startMoveCount, startMoveDoubt[0], startMoveDoubt[1]);
    ImGui::SliderInt("Count", &startMoveDoubt[0], 0, (int)std::pow(2,7));
    ImGui::SliderInt("Random", &startMoveDoubt[1], 0, (int)std::pow(2,7) -1);
    ImGui::SliderFloat("Wait", &minRandomTime, 0, 3);
    ImGui::Checkbox("AutoStart", &autoStart);
    if(fullRun != (fullRun = ImGui::Button("FullRun"))){
        GameRandomStart::reset();
    };
}

GameModeManager::GameModeManager(TTT_GridLogic& grid, TTT_GridView& view) {
    // Enregistrement des modes disponibles
    modeFactories["Classic"] = [&]() { return new GameState(grid, view); };
    modeFactories["RandomStart"] = [&]() { return new GameRandomStart(grid, view); };
    
    // Mode par défaut
    changeGameMode("Classic");
}

void GameModeManager::changeGameMode(const std::string& modeName) {
    if (modeFactories.find(modeName) != modeFactories.end()) {
        currentMode = modeFactories[modeName]();
        currentMode->reset();
    }
}