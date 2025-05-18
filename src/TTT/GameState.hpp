#pragma once

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
    GridShape currentPlayer;
    Path targetSubGridPath;
    std::vector<Move> moveHistory;
    std::vector<Move> redoHistory;

    GameState() { reset(); }

    static bool isBotPlayer(GridShape shape);
    bool playTurn(GridView& gridView);
    bool playBot(GridLogic& grid);
    bool playMove(Path path, GridLogic& grid, GridShape player);
    bool playMove(Path path, GridLogic& grid);
    bool playMoveBase(Path path, GridLogic& grid, GridShape player);
    void endTurn(const Path lastPlayedSubGridPath, GridLogic& grid);
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename, GridLogic& rootGrid);
    bool undoLastMove(GridLogic& rootGrid);
    bool redoLastMove(GridLogic& rootGrid);
    void reset();
};

std::ostream& operator<<(std::ostream& os, const Move& move);
std::ostream& operator<<(std::ostream& os, const GameState& gameState);