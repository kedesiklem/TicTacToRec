#include "TTT_MonteCarlo.hpp"

namespace TTT {

MCTSNode::MCTSNode(GameState* state, MCTSNode* parent, int move, std::mt19937* rng)
    : state(state), parent(parent), move(move), visits(0), wins(0), rng(rng) {}

MCTSNode::~MCTSNode() {
    delete state;
}

MCTSNode* MCTSNode::select(double explorationWeight) {
    MCTSNode* bestNode = nullptr;
    double bestValue = -std::numeric_limits<double>::max();

    for (auto& child : children) {
        double value = child->uctValue(explorationWeight);
        if (value > bestValue) {
            bestValue = value;
            bestNode = child.get();
        }
    }

    return bestNode;
}

MCTSNode* MCTSNode::expand() {
    auto availableMoves = state->getGrid().getAvailableMoves(state->targetSubGridPath);
    
    // Find first unexpanded move
    for (int moveIndex = 0; moveIndex < availableMoves.size(); ++moveIndex) {
        bool alreadyExpanded = false;
        for (auto& child : children) {
            if (child->move == moveIndex) {
                alreadyExpanded = true;
                break;
            }
        }
        
        if (!alreadyExpanded) {
            // Create new child node
            GameState* newState = new GameState(*state);
            auto movePath = availableMoves[moveIndex];
            newState->playMove(movePath);
            
            children.push_back(std::make_unique<MCTSNode>(
                newState, this, moveIndex
            ));
            return children.back().get();
        }
    }
    
    return nullptr; // All moves expanded
}

int MCTSNode::simulate(std::mt19937& rng) {
    GameState simState(*state);
    
    while (!simState.getGrid().isLocked()) {
        auto moves = simState.getGrid().getAvailableMoves(simState.targetSubGridPath);
        if (moves.empty()) break;
        
        std::uniform_int_distribution<> dist(0, moves.size() - 1);
        auto randomMove = moves[dist(rng)]; // Utilisation du RNG passé en paramètre
        simState.playMove(randomMove);
    }
    
    auto winner = simState.getGrid().checkVictory();
    if (winner == state->currentPlayer) return 1;
    if (winner == TTT_Shape::DRAW) return 0;
    return -1;
}

void MCTSNode::backpropagate(int result) {
    MCTSNode* node = this;
    while (node != nullptr) {
        node->visits++;
        node->wins += result;
        node = node->parent;
        result = -result; // Alternate perspective for players
    }
}

bool MCTSNode::isFullyExpanded() const {
    return children.size() == state->getGrid().getAvailableMoves(state->targetSubGridPath).size();
}

bool MCTSNode::isTerminal() const {
    return state->getGrid().isLocked();
}

double MCTSNode::uctValue(double explorationWeight) const {
    if (visits == 0) return std::numeric_limits<double>::max();
    return (wins / visits) + explorationWeight * std::sqrt(std::log(parent->visits) / visits);
}

int MCTSNode::getBestMove() const {
    int bestMove = -1;
    double bestValue = -std::numeric_limits<double>::max();

    for (auto& child : children) {
        double value = child->visits;
        if (value > bestValue) {
            bestValue = value;
            bestMove = child->move;
        }
    }

    return bestMove;
}

MonteCarloAI::MonteCarloAI(int iterations, double explorationWeight)
    : iterations(iterations), explorationWeight(explorationWeight),
      rng(std::chrono::system_clock::now().time_since_epoch().count()) {}

Path MonteCarloAI::findBestMove(GameState& initialState) {
    auto root = buildTree(initialState);
    int bestMoveIndex = root->getBestMove();
    
    if (bestMoveIndex == -1) {
        throw std::runtime_error("No valid moves found");
    }
    
    auto availableMoves = initialState.getGrid().getAvailableMoves(initialState.targetSubGridPath);
    return availableMoves[bestMoveIndex];
}

std::unique_ptr<MCTSNode> MonteCarloAI::buildTree(GameState& initialState) {
    auto root = std::make_unique<MCTSNode>(new GameState(initialState), nullptr, -1, &rng);

    for (int i = 0; i < iterations; ++i) {
        // Selection
        MCTSNode* node = root.get();
        while (node->isFullyExpanded() && !node->isTerminal()) {
            node = node->select(explorationWeight);
        }

        // Expansion
        if (!node->isTerminal()) {
            node = node->expand();
            if (!node) continue; // Skip if no expansion possible
        }

        // Simulation
        int result = node->simulate(rng);

        // Backpropagation
        node->backpropagate(result);
    }

    return root;
}

} // namespace TTT