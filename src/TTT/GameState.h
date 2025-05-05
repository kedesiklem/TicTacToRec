#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Grid.h"

class GameState {
    public:
        GridShape currentPlayer = GridShape::CROSS;
        std::vector<int> targetSubGridPath;

        GameState() {targetSubGridPath.clear();}

        bool update(const ImVec2& window_pos, Grid& grid);
        bool update(const ImVec2& window_pos, Grid& grid, std::vector<int> currentPath, std::vector<int>& finalPath, int recursionLevel);
        void endTurn(const std::vector<int>& lastPlayedSubGridPath, Grid& grid);

        void reset() {
            currentPlayer = GridShape::CROSS;
            targetSubGridPath.clear();
        }
};


std::ostream& operator<<(std::ostream& os, const GameState& gameState);


#endif // GAMESTATE_H