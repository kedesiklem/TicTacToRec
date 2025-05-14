#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Grid.h"
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

        GameState() {reset();}

        bool update(const ImVec2& window_pos, Grid& grid); // wrapper
        bool update(const ImVec2& window_pos, Grid& grid, std::vector<int> currentPath, std::vector<int>& finalPath, int recursionLevel);
        bool handleLeafGrid(const ImVec2 &window_pos, Grid &grid, std::vector<int> &currentPath, std::vector<int> &finalPath);
        bool handleNonLeafGrid(const ImVec2 &window_pos, Grid &grid, std::vector<int> currentPath, std::vector<int> &finalPath, int recursionLevel);
        bool shouldSkipSubGrid(const Grid &grid, int r, int c, int recursionLevel) const;
        bool updateSubGrid(const ImVec2 &pos, Grid &grid, int r, int c, std::vector<int> &currentPath, std::vector<int> &finalPath, int recursionLevel);
        void endTurn(const std::vector<int> lastPlayedSubGridPath, Grid &grid);
        bool playMove(const std::vector<int>& path, GridShape player, Grid& rootGrid);
        bool saveState(const std::string& filename) const;
        bool loadState(const std::string& filename, Grid& rootGrid);

        bool undoLastMove(Grid &rootGrid);
        bool redoLastMove(Grid& rootGrid);

        void reset();
};

std::ostream& operator<<(std::ostream& os, const Move& move);
std::ostream& operator<<(std::ostream& os, const GameState& gameState);


#endif // GAMESTATE_H