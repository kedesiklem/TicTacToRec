#include "GameState.hpp"
#include "GridView.hpp"

GridShape defaultPlayer() {
    return GridShape::CROSS;
}

bool GameState::isBotPlayer(GridShape shape) {
    return false;
}

bool GameState::playBot(const ImVec2& window_pos, GridLogic& grid, std::vector<int> currentPath, 
    std::vector<int>& finalPath, int recursionLevel) {
    std::vector<std::vector<int>> moves = grid.getValidMoves(targetSubGridPath);
    return grid.playMove(moves[0], currentPlayer);
}

bool GameState::playMove(std::vector<int> path, GridLogic& grid)
{
    playMove(path, grid, currentPlayer);
}

bool GameState::playMove(std::vector<int> path, GridLogic& grid, GridShape player)
{
    redoHistory.clear();
    return playMoveBase(path, grid, player);
}

bool GameState::playMoveBase(std::vector<int> path, GridLogic& grid, GridShape player)
{
    if(starts_with(path, targetSubGridPath))
        if(grid.playMove(path, player)){
            endTurn(path, grid);
            return true;
        }
    return false;
}

void GameState::endTurn(const std::vector<int> lastPlayedSubGridPath, GridLogic& grid) {
    moveHistory.push_back({lastPlayedSubGridPath, targetSubGridPath, currentPlayer});
    currentPlayer = GridLogic::nextShapePlayable(currentPlayer);

    if (lastPlayedSubGridPath.empty()) {
        targetSubGridPath.clear();
        return;
    }

    targetSubGridPath = lastPlayedSubGridPath;
    targetSubGridPath.erase(targetSubGridPath.begin());
    
    std::vector<int> currentPath;
    for (size_t i = 0; i < targetSubGridPath.size(); ++i) {
        currentPath.push_back(targetSubGridPath[i]);        
        if (grid.getGridFromPath(currentPath).getShape() != GridShape::NONE) {
            currentPath.pop_back();
            targetSubGridPath.assign(currentPath.begin(), currentPath.end());
            break;
        }
    }
}

void GameState::reset() {
    currentPlayer = defaultPlayer();
    targetSubGridPath.clear();
    moveHistory.clear();
    redoHistory.clear();
}

bool GameState::undoLastMove(GridLogic& rootGrid) {
    if (moveHistory.empty()) return false;

    Move lastMove = moveHistory.back();
    redoHistory.push_back(lastMove);
    moveHistory.pop_back();

    targetSubGridPath = lastMove.target;
    currentPlayer = lastMove.shape;


    GridLogic* currentGrid = &rootGrid;
    currentGrid->setShape(GridShape::NONE);

    for (size_t i = 0; i < lastMove.path.size(); ++i) {
        int index = lastMove.path[i];
        int rows = currentGrid->getRows();
        int cols = currentGrid->getCols();
        int r = index / cols;
        int c = index % cols;

        currentGrid = &currentGrid->getSubGrid(r, c);
        currentGrid->setShape(GridShape::NONE);
        

    }

    return true;
}

bool GameState::redoLastMove(GridLogic& rootGrid) {
    if (redoHistory.empty()) return false;

    Move redoMove = redoHistory.back();
    redoHistory.pop_back();

    targetSubGridPath = redoMove.target;
    currentPlayer = redoMove.shape;

    return playMoveBase(redoMove.path, rootGrid, redoMove.shape);
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
            outFile << ">" << GridLogic::GridShapeToString(move.shape);
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
            outFile << ">" << GridLogic::GridShapeToString(move.shape);
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

bool GameState::loadState(const std::string& filename, GridLogic& rootGrid) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    reset();
    rootGrid.resetGrid();

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
                GridShape shape = GridLogic::StringToGridShape(targetPos == std::string::npos ? rest : rest.substr(0, targetPos));
                
                std::vector<int> path;
                if (pathStr != "root") {
                    std::istringstream pathIss(pathStr);
                    std::string token;
                    while (std::getline(pathIss, token, '.')) {
                        path.push_back(std::stoi(token));
                    }
                }

                std::vector<int> target;
                if (targetPos != std::string::npos) {
                    std::string targetStr = rest.substr(targetPos + 1);
                    std::istringstream targetIss(targetStr);
                    std::string token;
                    while (std::getline(targetIss, token, '.')) {
                        target.push_back(std::stoi(token));
                    }
                }

                if (!playMove(path, rootGrid, shape)) {
                    success = false;
                }
            }
            else if (section == "RedoHistory") {
                size_t shapePos = line.find('>');
                if (shapePos == std::string::npos) continue;

                std::string pathStr = line.substr(0, shapePos);
                std::string rest = line.substr(shapePos + 1);
                
                size_t targetPos = rest.find('@');
                GridShape shape = GridLogic::StringToGridShape(targetPos == std::string::npos ? rest : rest.substr(0, targetPos));
                
                std::vector<int> path;
                if (pathStr != "root") {
                    std::istringstream pathIss(pathStr);
                    std::string token;
                    while (std::getline(pathIss, token, '.')) {
                        path.push_back(std::stoi(token));
                    }
                }

                std::vector<int> target;
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

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << move.shape << " ";
    for(size_t i = 0; i < move.path.size() - 1; ++i) {
        os << move.path[i] << ",";
    }
    if(move.path.size() > 0) {
        os << move.path[move.path.size() - 1];
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

    os << "[Move History][" << gameState.moveHistory.size() << "]" << std::endl;
    for (const auto& move : gameState.moveHistory) {
        os << "    " << move << " " << std::endl;
    }
    return os;
}