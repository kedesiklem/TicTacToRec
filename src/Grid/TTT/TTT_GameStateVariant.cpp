#include "TTT_GameStateVariant.hpp"

using namespace TTT;


bool GameStateBot::isBotPlayer(TTT_Shape shape) {
    return (autoMode);
}

bool GameStateBot::playBot(){
    auto move = bot->getNextMove(*this);
    if(move){
        return playMove(move.value());
    }
    return false;
}

bool GameStateBot::playTurn() {
    if(isBotPlayer(currentPlayer)) {
        return playBot();
    } else {
        auto path = view.handleGridInteraction(grid);
        if(path) {
            return playMove(path.value());
        }
        return false;
    }
}
