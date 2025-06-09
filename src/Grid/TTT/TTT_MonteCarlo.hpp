#pragma once

#include "TTT_GameState.hpp"
#include <memory>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>

namespace TTT {

class MCTSNode {
public:
    MCTSNode(GameState* state, MCTSNode* parent = nullptr, int move = -1, std::mt19937* rng = nullptr);
    ~MCTSNode();

    MCTSNode* select(double explorationWeight);
    MCTSNode* expand();
    int simulate(std::mt19937& rng);
    void backpropagate(int result);

    bool isFullyExpanded() const;
    bool isTerminal() const;
    double uctValue(double explorationWeight) const;
    
    int getBestMove() const;
    int getMove() const { return move; }
    int getVisits() const { return visits; }
    double getWins() const { return wins; }

private:
    GameState* state;
    MCTSNode* parent;
    std::vector<std::unique_ptr<MCTSNode>> children;
    int move; // The move that led to this node
    int visits;
    double wins;
    std::mt19937* rng;
};

class MonteCarloAI {
public:
    MonteCarloAI(int iterations = 1000, double explorationWeight = 1.41);
    Path findBestMove(GameState& initialState);

    void setIterations(int newIterations) { iterations = newIterations; }
    int getIterations() const { return iterations; }
    void setExplorationWeight(double newWeight) { explorationWeight = newWeight; }
    double getExplorationWeight() const { return explorationWeight; }

private:
    int iterations;
    double explorationWeight;
    std::mt19937 rng;

    std::unique_ptr<MCTSNode> buildTree(GameState& initialState);
};

} // namespace TTT