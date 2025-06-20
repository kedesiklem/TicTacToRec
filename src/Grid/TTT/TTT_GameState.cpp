#include "TTT_GameState.hpp"

using namespace TTT;

bool GameState::playMove(Path path) {
    return playMove(path, currentPlayer);
}

bool GameState::playMove(Path path, TTT_Shape player) {
    redoHistory.clear();
    return playMoveBase(path, player);
}

bool GameState::playMoveBase(Path path, TTT_Shape player) {
    if(starts_with(path, targetSubGridPath)) {
        if(grid.playMove(path, player)) {
            endTurn(path);
            return true;
        }
    }
    return false;
}

bool GameState::playRandom() {
    auto move = grid.getRandomAvailableMove(targetSubGridPath);
    if(move) {
        return playMove(move.value(), currentPlayer);
    } else {
        throw "No available move";
    }
}

bool GameState::playTurn() {
        auto path = view.handleGridInteraction(grid);
        if(path) {
            return playMove(path.value());
        }
        return false;
}

void GameState::endTurn(const Path lastPlayedSubGridPath) {
    moveHistory.push_back({lastPlayedSubGridPath, targetSubGridPath, currentPlayer});
    currentPlayer = TTT_Cell::nextShapePlayable(currentPlayer);

    if (lastPlayedSubGridPath.empty()) {
        targetSubGridPath.clear();
        return;
    }

    targetSubGridPath = lastPlayedSubGridPath;
    targetSubGridPath.erase(targetSubGridPath.begin());
    
    Path currentPath;
    for (size_t i = 0; i < targetSubGridPath.size(); ++i) {
        currentPath.push_back(targetSubGridPath[i]);        
        if (grid.getGridFromPath(currentPath).isLocked()) {
            currentPath.pop_back();
            targetSubGridPath.assign(currentPath.begin(), currentPath.end());
            break;
        }
    }
}

void GameState::reset() {
    grid.resetGrid();
    currentPlayer = defaultPlayer();
    targetSubGridPath.clear();
    moveHistory.clear();
    redoHistory.clear();
}

bool GameState::undoLastMove() {
    if (moveHistory.empty()) return false;

    TTT_Move lastMove = moveHistory.back();
    redoHistory.push_back(lastMove);
    moveHistory.pop_back();

    targetSubGridPath = lastMove.target;
    currentPlayer = lastMove.shape;

    TTT_GridLogic* currentGrid = &grid;
    currentGrid->setShape(TTT_Shape::NONE);

    for (size_t i = 0; i < lastMove.path.size(); ++i) {
        int index = lastMove.path[i];
        int cols = currentGrid->getCols();
        int r = index / cols;
        int c = index % cols;

        currentGrid = &currentGrid->getSubGrid(r, c);
        currentGrid->setShape(TTT_Shape::NONE);
    }

    return true;
}

bool GameState::redoLastMove() {
    if (redoHistory.empty()) return false;

    TTT_Move redoMove = redoHistory.back();
    redoHistory.pop_back();

    targetSubGridPath = redoMove.target;
    currentPlayer = redoMove.shape;

    return playMoveBase(redoMove.path, redoMove.shape);
}

bool GameState::saveState(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        return false;
    }

    try {
        outFile << "# Ultimate Tic-Tac-Toe Game State\n";
        outFile << "[MoveHistory]\n";
        outFile << "# Format: PATH>SHAPE@TARGET (SHAPE uses enum names)\n";
        for (const auto& move : moveHistory) {
            if (move.path.empty()) {
                outFile << "root";
            } else {
                for (size_t i = 0; i < move.path.size(); ++i) {
                    if (i > 0) outFile << ".";
                    outFile << move.path[i];
                }
            }
            outFile << ">" << TTT_Cell::ShapeToString(move.shape);
            if (!move.target.empty()) {
                outFile << "@";
                for (size_t i = 0; i < move.target.size(); ++i) {
                    if (i > 0) outFile << ".";
                    outFile << move.target[i];
                }
            }
            outFile << "\n";
        }
        outFile << "\n";

        outFile << "[RedoHistory]\n";
        for (const auto& move : redoHistory) {
            if (move.path.empty()) {
                outFile << "root";
            } else {
                for (size_t i = 0; i < move.path.size(); ++i) {
                    if (i > 0) outFile << ".";
                    outFile << move.path[i];
                }
            }
            outFile << ">" << TTT_Cell::ShapeToString(move.shape);
            if (!move.target.empty()) {
                outFile << "@";
                for (size_t i = 0; i < move.target.size(); ++i) {
                    if (i > 0) outFile << ".";
                    outFile << move.target[i];
                }
            }
            outFile << "\n";
        }

        return outFile.good();
    } catch (...) {
        return false;
    }
}

bool GameState::loadState(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    reset();

    std::string line;
    std::string section;
    bool success = true;

    try {
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
                continue;
            }

            if (section == "MoveHistory") {
                size_t shapePos = line.find('>');
                if (shapePos == std::string::npos) continue;

                std::string pathStr = line.substr(0, shapePos);
                std::string rest = line.substr(shapePos + 1);
                
                size_t targetPos = rest.find('@');
                TTT_Shape shape = TTT_Cell::StringToShape(targetPos == std::string::npos ? rest : rest.substr(0, targetPos));
                
                Path path;
                if (pathStr != "root") {
                    std::istringstream pathIss(pathStr);
                    std::string token;
                    while (std::getline(pathIss, token, '.')) {
                        path.push_back(std::stoi(token));
                    }
                }

                Path target;
                if (targetPos != std::string::npos) {
                    std::string targetStr = rest.substr(targetPos + 1);
                    std::istringstream targetIss(targetStr);
                    std::string token;
                    while (std::getline(targetIss, token, '.')) {
                        target.push_back(std::stoi(token));
                    }
                }

                if (!playMove(path, shape)) {
                    success = false;
                }
            }
            else if (section == "RedoHistory") {
                size_t shapePos = line.find('>');
                if (shapePos == std::string::npos) continue;

                std::string pathStr = line.substr(0, shapePos);
                std::string rest = line.substr(shapePos + 1);
                
                size_t targetPos = rest.find('@');
                TTT_Shape shape = TTT_Cell::StringToShape(targetPos == std::string::npos ? rest : rest.substr(0, targetPos));
                
                Path path;
                if (pathStr != "root") {
                    std::istringstream pathIss(pathStr);
                    std::string token;
                    while (std::getline(pathIss, token, '.')) {
                        path.push_back(std::stoi(token));
                    }
                }

                Path target;
                if (targetPos != std::string::npos) {
                    std::string targetStr = rest.substr(targetPos + 1);
                    std::istringstream targetIss(targetStr);
                    std::string token;
                    while (std::getline(targetIss, token, '.')) {
                        target.push_back(std::stoi(token));
                    }
                }

                redoHistory.push_back({path, target, shape});
            }
        }
    } catch (...) {
        success = false;
    }

    return success && inFile.eof();
}

std::ostream& operator<<(std::ostream& os, const TTT_Move& move) {
    try {
        os << move.shape << " ";
        if (!move.path.empty()) {
            os << move.path.at(0);
            for (size_t i = 1; i < move.path.size(); ++i) {
                os << "," << move.path.at(i);
            }
        }
    }
    catch (const std::out_of_range& e) {
        os.setstate(std::ios_base::failbit);
        throw std::runtime_error("Invalid path index in TTT_Move serialization: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        os.setstate(std::ios_base::failbit);
        throw;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const GameState& gameState) {
    os << "GameState: " << std::endl;
    os << "Current Player: " << gameState.currentPlayer << std::endl;
    os << "Target SubGrid Path: ";
    for (const auto& path : gameState.targetSubGridPath) {
        os << path << " ";
    }
    os << std::endl;

    os << "[TTT_Move History][" << gameState.moveHistory.size() << "]" << std::endl;
    for (const auto& move : gameState.moveHistory) {
        os << "    " << move << " " << std::endl;
    }
    return os;
}