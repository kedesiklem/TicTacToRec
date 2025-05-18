#include "GridLogic.hpp"


GridLogic::GridLogic(int rows, int cols, int depth) : rows(rows), cols(cols) {
    if (rows > 1 || cols > 1) {
        subGrids.resize(rows);
        for (int r = 0; r < rows; ++r) {
            subGrids[r].reserve(cols);
            for (int c = 0; c < cols; ++c) {
                if(depth > 0) {
                    subGrids[r].emplace_back(rows, cols, depth - 1);
                } else {
                    subGrids[r].emplace_back(1, 1, 0);
                }
            }
        }
    }
}

GridShape GridLogic::nextShapePlayable(GridShape shape){
    switch (shape){
        case GridShape::CROSS:
            return GridShape::CIRCLE;
        
        case GridShape::CIRCLE:
            return GridShape::CROSS;

        default:
            return shape;
    }
}

std::string GridLogic::GridShapeToString(GridShape shape) {
    switch(shape) {
        case GridShape::NONE: return "NONE";
        case GridShape::CROSS: return "CROSS";
        case GridShape::CIRCLE: return "CIRCLE";
        case GridShape::DRAW: return "DRAW";
        default: return "UNKNOWN";
    }
}

GridShape GridLogic::StringToGridShape(const std::string& str) {
    if (str == "NONE") return GridShape::NONE;
    if (str == "CROSS") return GridShape::CROSS;
    if (str == "CIRCLE") return GridShape::CIRCLE;
    if (str == "DRAW") return GridShape::DRAW;
    return GridShape::NONE; // Valeur par défaut
}

void GridLogic::resetGrid() {
    currentShape = GridShape::NONE;
    for (auto& row : subGrids) {
        for (auto& subGrid : row) {
            subGrid.resetGrid();
        }
    }
}

GridLogic& GridLogic::getSubGrid(int index){
    if (index < 0 || index >= rows * cols) {
        throw std::out_of_range("Invalid subgrid indices");
    }

    int r = index / cols;
    int c = index % cols;

    return getSubGrid(r,c);
};

GridLogic& GridLogic::getSubGrid(size_t row, size_t col) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return subGrids[row][col];
    }
    throw std::out_of_range("Invalid subgrid indices");
};

const GridLogic& GridLogic::getSubGrid(size_t row, size_t col) const {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return subGrids[row][col];
    }
    throw std::out_of_range("Invalid subgrid indices");
};


GridLogic GridLogic::getGridFromPath(const std::vector<int>& path) {
    GridLogic* grid = this;
    for (int index : path) {
        int row = index / cols;
        int col = index % cols;
        grid = &grid->subGrids[row][col];
    }
    return *grid;
}

GridShape GridLogic::checkVictory(){
    if(isLeaf()){
        return currentShape;
    }
    // Vérification des lignes
    for (int r = 0; r < rows; ++r) {
        GridShape first = getSubGrid(r, 0).currentShape;
        if (first == GridShape::NONE) continue;
        
        bool win = true;
        for (int c = 1; c < cols; ++c) {
            if (getSubGrid(r, c).currentShape != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    // Vérification des colonnes
    for (int c = 0; c < cols; ++c) {
        GridShape first = getSubGrid(0, c).currentShape;
        if (first == GridShape::NONE) continue;
        
        bool win = true;
        for (int r = 1; r < rows; ++r) {
            if (getSubGrid(r, c).currentShape != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    // Vérification de la diagonale principale
    GridShape first = getSubGrid(0, 0).currentShape;
    if (first != GridShape::NONE) {
        bool win = true;
        for (int i = 1; i < rows; ++i) {
            if (getSubGrid(i, i).currentShape != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    // Vérification de l'anti-diagonale
    first = getSubGrid(0, cols-1).currentShape;
    if (first != GridShape::NONE) {
        bool win = true;
        for (int i = 1; i < rows; ++i) {
            if (getSubGrid(i, cols-1-i).currentShape != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    // Vérification du match nul
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (getSubGrid(r, c).currentShape == GridShape::NONE) {
                return GridShape::NONE;
            }
        }
    }

    return GridShape::DRAW;
}

std::vector<std::vector<int>> GridLogic::getValidMoves(std::vector<int> target) {
    std::vector<std::vector<int>> validMoves;
    
    // Si la cible est vide, on peut jouer n'importe où
    if (target.empty()) {
        if (isLeaf()) {
            // Si c'est une feuille et non verrouillée, c'est un coup valide
            if (!isLockedShaped()) {
                validMoves.push_back({}); // Coup vide signifie cette cellule
            }
        } else {
            // Pour chaque sous-grille non verrouillée
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    if (!subGrids[r][c].isLockedShaped()) {
                        // Récupère les coups valides de la sous-grille
                        auto subMoves = subGrids[r][c].getValidMoves({});
                        for (auto& move : subMoves) {
                            move.insert(move.begin(), r * cols + c);
                            validMoves.push_back(move);
                        }
                    }
                }
            }
        }
        return validMoves;
    }

    // Si la cible pointe vers cette grille
    if (target.size() == 1) {
        int index = target[0];
        if (index < 0 || index >= rows * cols) {
            return validMoves; // Cible invalide
        }

        int r = index / cols;
        int c = index % cols;

        if (subGrids[r][c].isLockedShaped()) {
            // Si la sous-grille cible est verrouillée, on peut jouer n'importe où
            return getValidMoves({});
        }

        // Récupère les coups valides dans la sous-grille cible
        auto subMoves = subGrids[r][c].getValidMoves({});
        for (auto& move : subMoves) {
            move.insert(move.begin(), index);
            validMoves.push_back(move);
        }
        return validMoves;
    }

    // Si la cible est plus profonde, on délègue à la sous-grille appropriée
    int index = target[0];
    if (index < 0 || index >= rows * cols) {
        return validMoves; // Cible invalide
    }

    int r = index / cols;
    int c = index % cols;
    std::vector<int> remainingTarget(target.begin() + 1, target.end());
    
    auto subMoves = subGrids[r][c].getValidMoves(remainingTarget);
    for (auto& move : subMoves) {
        move.insert(move.begin(), index);
        validMoves.push_back(move);
    }

    return validMoves;
}

bool GridLogic::playMove(const std::vector<int> &path, GridShape player){
    return playMove(path, player, 0, false);
}

bool GridLogic::playMove(const std::vector<int> &path, GridShape player, size_t step, bool locked, std::vector<int> currentPath)
{
    locked |= isLockedShaped();
    locked |= !starts_with(currentPath, path);

    if(isLeaf()){
        if(!locked){
            setShape(player);
            return true;
        }
    }else{
        currentPath.push_back(path[step]);
        if(getSubGrid(path[step]).playMove(path, player, step +1, locked, currentPath)){
            if(checkVictory() == player) setShape(player);
            return true;
        }
    }
    return false;
}

void GridLogic::undoMove(const std::vector<int> &path, size_t step)
{
    if(step == path.size()){
        if(isLeaf() && isLockedShaped()){
            setShape(GridShape::NONE);
        }else{
            throw("Invalid Path");
        }
    }else{
        setShape(GridShape::NONE);
        getSubGrid(path[step]).undoMove(path, step +1);
    }
};

std::ostream& operator<<(std::ostream& os, const GridShape& shape){ 
    switch (shape){
        case GridShape::NONE   :    os << "None"; break;
        case GridShape::CROSS  :    os << "X";    break;
        case GridShape::CIRCLE :    os << "O";    break;
        case GridShape::DRAW   :    os << "-";    break;

        default: os << "Undefined"; break;
    }
    return os;
};
