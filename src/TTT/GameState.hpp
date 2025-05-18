#pragma once

#include "GridView.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

struct Move {
    std::vector<int> path;
    std::vector<int> target;
    GridShape shape;
};

class GameState {
public:
    GridShape currentPlayer;
    std::vector<int> targetSubGridPath;
    std::vector<Move> moveHistory;
    std::vector<Move> redoHistory;

    GameState() { reset(); }

    static bool isBotPlayer(GridShape shape);
    bool playBot(const ImVec2& window_pos, GridLogic& grid, std::vector<int> currentPath, std::vector<int>& finalPath, int recursionLevel);
    bool playMove(std::vector<int> path, GridLogic& grid, GridShape player);
    bool playMove(std::vector<int> path, GridLogic& grid);
    bool playMoveBase(std::vector<int> path, GridLogic& grid, GridShape player);
    void endTurn(const std::vector<int> lastPlayedSubGridPath, GridLogic& grid);
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename, GridLogic& rootGrid);
    bool undoLastMove(GridLogic& rootGrid);
    bool redoLastMove(GridLogic& rootGrid);
    void reset();
};

std::ostream& operator<<(std::ostream& os, const Move& move);
std::ostream& operator<<(std::ostream& os, const GameState& gameState);