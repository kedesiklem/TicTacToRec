#include "MonteCarloBot.hpp"


Path MonteCarloBot::getMonteCarloBotMove(GridLogic& grid, const GridShape& bot, Path& target) {
    std::vector<Path> moves = grid.getAvailableMove(target);

    // Évalue chaque coup possible avec Monte Carlo
    Path bestMove;
    double bestScore = -INFINITY;

    for (const auto& move : moves) {
        double score = evaluateMoveWithMonteCarlo(move, grid, bot, 1000);
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}


double MonteCarloBot::evaluateMoveWithMonteCarlo(const Path& move, GridLogic& grid, const GridShape& bot, int simulations) {
    int wins = 0;
    GridLogic tempGrid = grid; // Copie de la grille pour simulation

    // Joue le coup initial
    tempGrid.playMove(move, bot);

    // Simule 'simulations' parties aléatoires
    for (int i = 0; i < simulations; ++i) {
        GridLogic simulationGrid = tempGrid; // Copie pour chaque simulation
        GridShape simBot = GridLogic::nextShapePlayable(bot);
        bool gameOver = false;

        while (!gameOver) {
            std::vector<Path> availableMoves = simulationGrid.getAvailableMove({});
            if (availableMoves.empty()) {
                gameOver = true;
                break;
            }

            // Joue un coup aléatoire
            Path randomMove = availableMoves[rand() % availableMoves.size()];
            simulationGrid.playMove(randomMove, simBot);

            // Vérifie si la partie est terminée
            GridShape result = simulationGrid.checkVictory();

            int score = 0;
            if(result == bot) {score = 1;}
            else if (GridLogic::isWinningShape(result)) {score = -1;}

            wins += score;

            if (result != GridShape::NONE) {
                gameOver = true;
            }

            simBot = GridLogic::nextShapePlayable(simBot);
        }
    }

    return static_cast<double>(wins) / simulations;
}