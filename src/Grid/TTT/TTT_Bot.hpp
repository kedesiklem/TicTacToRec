#pragma once

#include "TTT_GameState.hpp"
#include <optional>

namespace TTT {

class TTT_Bot {
public:
    virtual ~TTT_Bot() = default;
    virtual std::optional<Path> getNextMove(const GameState& gameState) = 0;
};

class RandomBot : public TTT_Bot {
public:
    std::optional<Path> getNextMove(const GameState& gameState) override {
        return gameState.grid.getRandomAvailableMove(gameState.targetSubGridPath);
    }
};

class FirstMoveBot : public TTT_Bot {
public:
    std::optional<Path> getNextMove(const GameState& gameState) override {
        auto moves = gameState.grid.getAvailableMoves(gameState.targetSubGridPath);
        if(moves.empty())
            return std::nullopt;
        return moves[0];
    }
};

// À implémenter plus tard
class MinimaxBot : public TTT_Bot {
public:
    std::optional<Path> getNextMove(const GameState& gameState) override;
};

class MonteCarloBot : public TTT_Bot {
public:
    std::optional<Path> getNextMove(const GameState& gameState) override;
};

} // namespace TTT