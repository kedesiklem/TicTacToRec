#pragma once

#include "TTT_GridView.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace TTT {

struct TTT_Move {
    Path path;
    Path target;
    TTT_Shape shape;
};

class GameState {
public:
    bool autoMode = false;
    TTT_GridView& grid;
    TTT_Shape currentPlayer;
    Path targetSubGridPath;
    std::vector<TTT_Move> moveHistory;
    std::vector<TTT_Move> redoHistory;

    GameState(TTT_GridView& grid) : grid(grid) { reset(); }

    virtual void showParam(){};

    bool isBotPlayer(TTT_Shape shape);
    bool playMove(Path path, TTT_Shape player);
    bool playMove(Path path);
    bool playMoveBase(Path path, TTT_Shape player);
    bool playRandom();
    virtual bool playTurn();
    void endTurn(const Path lastPlayedSubGridPath);
    
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    bool undoLastMove();
    bool redoLastMove();
    
    virtual void reset();
};

} // namespace TTT

std::ostream& operator<<(std::ostream& os, const TTT::TTT_Move& move);
std::ostream& operator<<(std::ostream& os, const TTT::GameState& gameState);

