#include "GameModeManager.hpp"

bool GameRandomStart::playTurn() {
    if(fullRun){
        while(!grid.grid_root.isLockedShaped()){playRandom();};
        fullRun = false;
        return true;
    }else if((isBotPlayer(currentPlayer)) || ((startMoveCount > moveHistory.size()) && autoStart)){
        return playRandom();
    }else{
        auto path = grid.handleGridInteraction();
        if(path){
            return playMove(path.value());
        }
    }
}

bool GameRandomStart::playRandom() {
    if(!fullRun){
        usleep(minRandomTime);
    }
    return GameState::playRandom();
}

void GameRandomStart::reset(){
    startMoveCount = startMoveDoubt[0];
    if(startMoveDoubt[1] > 0){
        startMoveCount += rand() % (1 + startMoveDoubt[1]);
    }
    GameState::reset();
}

void GameRandomStart::showParam(){
    ImGui::Text("%ld,%ld,%ld", startMoveCount, startMoveDoubt[0], startMoveDoubt[1]);
    ImGui::SliderInt("RandomCount", &startMoveDoubt[0], 0, 100);
    ImGui::SliderInt("RandomDoubt", &startMoveDoubt[1], 0, 20);
    ImGui::Checkbox("AutoStart", &autoStart);
    if(fullRun != (fullRun = ImGui::Button("FullRun"))){
        GameRandomStart::reset();
    };
}

GameModeManager::GameModeManager(GridView& grid) {
    // Enregistrement des modes disponibles
    modeFactories["Classic"] = [&]() { return new GameState(grid); };
    modeFactories["RandomStart"] = [&]() { return new GameRandomStart(grid); };
    
    // Mode par défaut
    changeGameMode("Classic");
}

void GameModeManager::changeGameMode(const std::string& modeName) {
    if (modeFactories.find(modeName) != modeFactories.end()) {
        currentMode = modeFactories[modeName]();
        currentMode->reset();
    }
}