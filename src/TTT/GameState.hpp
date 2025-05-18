#pragma once

#include "MonteCarloBot.hpp"
#include "GridView.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

struct Move {
    Path path;
    Path target;
    GridShape shape;
};

class GameState {
public:

    MonteCarloBot& bot;
    GridView& grid;
    GridShape currentPlayer;
    Path targetSubGridPath;
    std::vector<Move> moveHistory;
    std::vector<Move> redoHistory;

    GameState(GridView& grid, MonteCarloBot& bot) : grid(grid), bot(bot) { reset(); }

    static bool isBotPlayer(GridShape shape);

    bool playMove(Path path, GridShape player);
    bool playMove(Path path);
    bool playMoveBase(Path path, GridShape player);

    bool playBot();

    bool playTurn();
    

    void endTurn(const Path lastPlayedSubGridPath);
    
    
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    
    bool undoLastMove();
    bool redoLastMove();
    
    void reset();
};

std::ostream& operator<<(std::ostream& os, const Move& move);
std::ostream& operator<<(std::ostream& os, const GameState& gameState);