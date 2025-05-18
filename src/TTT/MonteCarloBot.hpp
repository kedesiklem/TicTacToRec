#pragma once

#include "GridLogic.hpp"



class MonteCarloBot{
    public:
        Path getMonteCarloBotMove(GridLogic& grid, const GridShape& bot, Path& target);
        double evaluateMoveWithMonteCarlo(const Path& move, GridLogic& grid, const GridShape& bot, int simulations);

};