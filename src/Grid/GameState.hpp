#pragma once

#include "GridView.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

struct Move {
    Path path;
    Path target;
    TTT_Shape shape;
};

class GameState {
public:
    bool autoMode = false;
    GridView& grid;
    TTT_Shape currentPlayer;
    Path targetSubGridPath;
    std::vector<Move> moveHistory;
    std::vector<Move> redoHistory;

    GameState(GridView& grid) : grid(grid) { reset(); }

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

std::ostream& operator<<(std::ostream& os, const Move& move);
std::ostream& operator<<(std::ostream& os, const GameState& gameState);