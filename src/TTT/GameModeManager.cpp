#include "GameModeManager.hpp"

bool GameRandomStart::playTurn() {
    if((isBotPlayer(currentPlayer)) || (startMoveCount > moveHistory.size())){
        return playRandom();
    }else{
        auto path = grid.handleGridInteraction();
        if(path){
            return playMove(path.value());
        }
    }
}

bool GameRandomStart::playRandom() {
    usleep(minRandomTime);
    GameState::playRandom();
}