#include "TTT_Bot.hpp"

namespace TTT {

// Exemple d'implémentation MinimaxBot (simplifiée)
std::optional<Path> MinimaxBot::getNextMove(const GameState& gameState) {
    // TODO: Implémenter Minimax avec évaluation heuristique
    // Pour l'instant, on utilise un random comme placeholder
    return gameState.grid.getRandomAvailableMove(gameState.targetSubGridPath);
}

// Exemple d'implémentation MonteCarloBot (simplifiée)
std::optional<Path> MonteCarloBot::getNextMove(const GameState& gameState) {
    // TODO: Implémenter MCTS avec simulations
    // Pour l'instant, on utilise un random comme placeholder
    return gameState.grid.getRandomAvailableMove(gameState.targetSubGridPath);
}

} // namespace TTT