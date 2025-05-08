#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Grid.h"

struct Move {
    std::vector<int> path;
    std::vector<int> previousTarget;
    GridShape previousShape;
};

class GameState {

    public:
        GridShape currentPlayer = GridShape::CROSS;
        std::vector<int> targetSubGridPath;
        std::vector<Move> moveHistory;

        GameState() {targetSubGridPath.clear();}

        bool update(const ImVec2& window_pos, Grid& grid); // wrapper
        bool update(const ImVec2& window_pos, Grid& grid, std::vector<int> currentPath, std::vector<int>& finalPath, int recursionLevel);
        bool handleLeafGrid(const ImVec2 &window_pos, Grid &grid, std::vector<int> &currentPath, std::vector<int> &finalPath);
        bool handleNonLeafGrid(const ImVec2 &window_pos, Grid &grid, std::vector<int> currentPath, std::vector<int> &finalPath, int recursionLevel);
        bool shouldSkipSubGrid(const Grid &grid, int r, int c, int recursionLevel) const;
        bool updateSubGrid(const ImVec2 &pos, Grid &grid, int r, int c, std::vector<int> &currentPath, std::vector<int> &finalPath, int recursionLevel);
        void endTurn(const std::vector<int> lastPlayedSubGridPath, Grid &grid);

        bool undoLastMove(Grid &rootGrid);

        void reset() {
            currentPlayer = GridShape::CROSS;
            targetSubGridPath.clear();
            moveHistory.clear();
        }
};


std::ostream& operator<<(std::ostream& os, const GameState& gameState);


#endif // GAMESTATE_H