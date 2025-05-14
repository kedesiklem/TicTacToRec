#include "GameState.h"


GridShape defaultPlayer(){
    return GridShape::CROSS;
}

bool isGridLocked(Grid& grid) {
    if (grid.isLockedShaped()) {
        grid.setHovered(false);
        return true;
    }
    return false;
}

bool isMouseOverGrid(const ImVec2& window_pos, const Grid& grid) {
    ImVec2 mouse_pos = ImGui::GetMousePos();
    size_t cellSize = grid.getCellSize();

    return (mouse_pos.x >= window_pos.x && mouse_pos.x < window_pos.x + cellSize &&
            mouse_pos.y >= window_pos.y && mouse_pos.y < window_pos.y + cellSize);
}

ImVec2 calculateSubGridPosition(const ImVec2& window_pos, const Grid& grid, int r, int c) {
    size_t cellSize = grid.getCellSize();
    size_t padding = grid.getPadding();

    return ImVec2(
        window_pos.x + c * (cellSize + padding),
        window_pos.y + r * (cellSize + padding)
    );
}

void checkAllVictories(Grid& grid) {
    if (grid.isLeaf()) return;

    for (int r = 0; r < grid.getRows(); ++r) {
        for (int c = 0; c < grid.getCols(); ++c) {
            checkAllVictories(grid.getSubGrid(r, c));
        }
    }
    grid.setShape(grid.checkVictory());
}

// wrapper pour update
bool GameState::update(const ImVec2& window_pos, Grid& grid){
    std::vector<int> currentPath = {};
    std::vector<int> finalPath = {};
    bool result = update(window_pos, grid, currentPath, finalPath, 0);
    if(result){
        redoHistory.clear();
    }
    return result;
}

bool GameState::update(const ImVec2& window_pos, Grid& grid, std::vector<int> currentPath, 
    std::vector<int>& finalPath, int recursionLevel) {
    if (isGridLocked(grid)) {
        return false;
    }

    if (grid.isLeaf()) {
        return handleLeafGrid(window_pos, grid, currentPath, finalPath);
    }

    return handleNonLeafGrid(window_pos, grid, currentPath, finalPath, recursionLevel);
}

bool GameState::handleLeafGrid(const ImVec2& window_pos, Grid& grid, 
            std::vector<int>& currentPath, std::vector<int>& finalPath) {
    grid.setHovered(false);

    if (isMouseOverGrid(window_pos, grid) && grid.getShape() == GridShape::NONE) {
        grid.setHovered(true);

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            grid.setShape(currentPlayer);
            finalPath = currentPath;
            return true;
        }
    }
    return false;
}

bool GameState::handleNonLeafGrid(const ImVec2& window_pos, Grid& grid, 
               std::vector<int> currentPath, std::vector<int>& finalPath, 
               int recursionLevel) {
    bool updated = false;

    for (int r = 0; r < grid.getRows(); ++r) {
        for (int c = 0; c < grid.getCols(); ++c) {
            if (shouldSkipSubGrid(grid, r, c, recursionLevel))
                continue;

            ImVec2 subGridPos = calculateSubGridPosition(window_pos, grid, r, c);
            currentPath.push_back(r * grid.getCols() + c);

            if (updateSubGrid(subGridPos, grid, r, c, currentPath, finalPath, recursionLevel)) {
            grid.setShape(grid.checkVictory());
            updated = true;
            }

            currentPath.pop_back();
        }
    }

    if (recursionLevel == 0 && updated) {
        endTurn(finalPath, grid);
    }

    return updated;
}

bool GameState::shouldSkipSubGrid(const Grid& grid, int r, int c, int recursionLevel) const {
    if (targetSubGridPath.empty() || recursionLevel >= targetSubGridPath.size()) {
        return false;
    }

    int index = targetSubGridPath[recursionLevel];
    int target_r = index / grid.getCols();
    int target_c = index % grid.getCols();

    return (r != target_r || c != target_c);
}

bool GameState::updateSubGrid(const ImVec2& pos, Grid& grid, int r, int c, 
           std::vector<int>& currentPath, std::vector<int>& finalPath, 
           int recursionLevel) {
    return update(pos, grid.getSubGrid(r, c), currentPath, finalPath, recursionLevel + 1);
}

void GameState::endTurn(const std::vector<int> lastPlayedSubGridPath, Grid& grid) {
    // Changement de joueur
    moveHistory.push_back({lastPlayedSubGridPath, targetSubGridPath, currentPlayer});
    currentPlayer = nextShapePlayable(currentPlayer);


    if (lastPlayedSubGridPath.empty()) {
        targetSubGridPath.clear();
        return;
    }

    // On commence par la grille cible complète
    targetSubGridPath = lastPlayedSubGridPath;
    targetSubGridPath.erase(targetSubGridPath.begin());
    
    // On cherche le premier niveau avec une shape non-NONE en partant de la racine
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

bool GameState::undoLastMove(Grid& rootGrid) {
    if (moveHistory.empty()) return false;

    Move lastMove = moveHistory.back();

    redoHistory.push_back(lastMove);
    moveHistory.pop_back();

    targetSubGridPath = lastMove.target;
    currentPlayer = lastMove.shape;

    // Parcourir la grille selon le chemin pour trouver la cellule à annuler
    Grid* currentGrid = &rootGrid;
    for (size_t i = 0; i < lastMove.path.size(); ++i) {
        int index = lastMove.path[i];
        int rows = currentGrid->getRows();
        int cols = currentGrid->getCols();
        int r = index / cols;
        int c = index % cols;
        
        if (i == lastMove.path.size() - 1) {
            // Dernière étape du chemin - restaurer la forme
            currentGrid->getSubGrid(r, c).setShape(GridShape::NONE);
        } else {
            currentGrid = &currentGrid->getSubGrid(r, c);
        }
    }

    checkAllVictories(rootGrid);
    
    return true;
}

bool GameState::redoLastMove(Grid& rootGrid) {
    if (redoHistory.empty()) return false;

    Move redoMove = redoHistory.back();
    redoHistory.pop_back();

    targetSubGridPath = redoMove.target;
    currentPlayer = redoMove.shape;

    playMove(redoMove.path, redoMove.shape, rootGrid);
}

bool GameState::saveState(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        return false;
    }

    try {
        // En-tête avec version
        outFile << "# Ultimate Tic-Tac-Toe Game State\n";

        // Current player
        outFile << "[CurrentPlayer]\n";
        outFile << GridShapeToString(currentPlayer) << "\n\n";

        // Target subgrid path
        outFile << "[TargetSubGrid]\n";
        if (targetSubGridPath.empty()) {
            outFile << "none\n";
        } else {
            for (size_t i = 0; i < targetSubGridPath.size(); ++i) {
                if (i > 0) outFile << ".";
                outFile << targetSubGridPath[i];
            }
            outFile << "\n\n";
        }

        // Move history
        outFile << "[MoveHistory]\n";
        outFile << "# Format: PATH>SHAPE@TARGET (SHAPE uses enum names)\n";
        for (const auto& move : moveHistory) {
            // Path
            if (move.path.empty()) {
                outFile << "root";
            } else {
                for (size_t i = 0; i < move.path.size(); ++i) {
                    if (i > 0) outFile << ".";
                    outFile << move.path[i];
                }
            }

            // Shape (utilise le nom de l'enum)
            outFile << ">" << GridShapeToString(move.shape);

            // Target
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

        // Redo history
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
            outFile << ">" << GridShapeToString(move.shape);
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

bool GameState::loadState(const std::string& filename, Grid& rootGrid) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    // Reset game state and grid
    reset();
    rootGrid.resetGrid();

    std::string line;
    std::string section;
    bool success = true;

    try {
        while (std::getline(inFile, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // Check for section headers
            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
                continue;
            }

            if (section == "CurrentPlayer") {
                currentPlayer = StringToGridShape(line);
            }
            else if (section == "TargetSubGrid") {
                if (line != "none") {
                    targetSubGridPath.clear();
                    std::istringstream iss(line);
                    std::string token;
                    while (std::getline(iss, token, '.')) {
                        targetSubGridPath.push_back(std::stoi(token));
                    }
                }
            }
            else if (section == "MoveHistory") {
                // Format: PATH>SHAPE@TARGET or PATH>SHAPE
                size_t shapePos = line.find('>');
                if (shapePos == std::string::npos) continue;

                std::string pathStr = line.substr(0, shapePos);
                std::string rest = line.substr(shapePos + 1);
                
                // Parse shape
                size_t targetPos = rest.find('@');
                GridShape shape = StringToGridShape(targetPos == std::string::npos ? rest : rest.substr(0, targetPos));
                
                // Parse path
                std::vector<int> path;
                if (pathStr != "root") {
                    std::istringstream pathIss(pathStr);
                    std::string token;
                    while (std::getline(pathIss, token, '.')) {
                        path.push_back(std::stoi(token));
                    }
                }

                // Parse target if exists
                std::vector<int> target;
                if (targetPos != std::string::npos) {
                    std::string targetStr = rest.substr(targetPos + 1);
                    std::istringstream targetIss(targetStr);
                    std::string token;
                    while (std::getline(targetIss, token, '.')) {
                        target.push_back(std::stoi(token));
                    }
                }

                // Play the move
                if (!playMove(path, shape, rootGrid)) {
                    success = false;
                }
            }
            else if (section == "RedoHistory") {
                // Same format as MoveHistory
                size_t shapePos = line.find('>');
                if (shapePos == std::string::npos) continue;

                std::string pathStr = line.substr(0, shapePos);
                std::string rest = line.substr(shapePos + 1);
                
                // Parse shape
                size_t targetPos = rest.find('@');
                GridShape shape = StringToGridShape(targetPos == std::string::npos ? rest : rest.substr(0, targetPos));
                
                // Parse path
                std::vector<int> path;
                if (pathStr != "root") {
                    std::istringstream pathIss(pathStr);
                    std::string token;
                    while (std::getline(pathIss, token, '.')) {
                        path.push_back(std::stoi(token));
                    }
                }

                // Parse target if exists
                std::vector<int> target;
                if (targetPos != std::string::npos) {
                    std::string targetStr = rest.substr(targetPos + 1);
                    std::istringstream targetIss(targetStr);
                    std::string token;
                    while (std::getline(targetIss, token, '.')) {
                        target.push_back(std::stoi(token));
                    }
                }

                // Store in redo history
                redoHistory.push_back({path, target, shape});
            }
        }
    } catch (...) {
        success = false;
    }

    return success && inFile.eof();
}

//Duplication par rapport à update
bool GameState::playMove(const std::vector<int>& path, GridShape player, Grid& rootGrid) {

    if (path.empty()) {
        return false;
    }

    Grid* targetGrid = &rootGrid;
    for (size_t i = 0; i < path.size(); ++i) {
        int index = path[i];
        int rows = targetGrid->getRows();
        int cols = targetGrid->getCols();
        
        if (index < 0 || index >= rows * cols) {
            return false;
        }

        int r = index / cols;
        int c = index % cols;

        if (i == path.size() - 1) {
            if (targetGrid->getSubGrid(r, c).getShape() != GridShape::NONE || 
                targetGrid->getSubGrid(r, c).isLockedShaped()) {
                return false;
            }

            targetGrid->getSubGrid(r, c).setShape(player);
            
            endTurn(path, rootGrid);

            checkAllVictories(rootGrid);
            
            return true;
        } else {
            targetGrid = &targetGrid->getSubGrid(r, c);
        }
    }

    return false;
}

std::ostream& operator<<(std::ostream& os, const Move& move){
    os << move.shape << " ";
    for(size_t i=0; i<move.path.size() - 1; ++i){
        os << move.path[i] << ",";
    }
    if(move.path.size() > 0){
        os << move.path[move.path.size() - 1];
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const GameState &gameState)
{
    os << "GameState: " << std::endl;
    os << "Current Player: " << gameState.currentPlayer << std::endl;
    os << "Target SubGrid Path: ";
    for (const auto &path : gameState.targetSubGridPath)
    {
        os << path << " ";
    }
    os << std::endl;

    os << "[Move History][" << gameState.moveHistory.size() << "]" << std::endl;
    for (const auto &move : gameState.moveHistory)
    {
        os << "    " << move << " " << std::endl;
        ;
    }
    return os;
}
