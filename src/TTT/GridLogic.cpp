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

GridShape GridLogic::nextShapePlayable(const GridShape shape){
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
        case GridShape::NONE: return "N";
        case GridShape::CROSS: return "X";
        case GridShape::CIRCLE: return "O";
        case GridShape::DRAW: return "=";
        default: return "?";
    }
}

GridShape GridLogic::StringToGridShape(const std::string& str) {
    if (str == "N") return GridShape::NONE;
    if (str == "X") return GridShape::CROSS;
    if (str == "O") return GridShape::CIRCLE;
    if (str == "=") return GridShape::DRAW;
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

const GridLogic& GridLogic::getSubGrid(int index) const {
    if (index < 0 || index >= rows * cols) {
        throw std::out_of_range("Invalid subgrid indices [const]");
    }

    int r = index / cols;
    int c = index % cols;

    return getSubGrid(r,c);
};

GridLogic& GridLogic::getSubGrid(int row, int col) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return subGrids[row][col];
    }
    throw std::out_of_range("Invalid subgrid indices");
};

const GridLogic& GridLogic::getSubGrid(int row, int col) const {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return subGrids[row][col];
    }
    throw std::out_of_range("Invalid subgrid indices [const]");
};

GridLogic GridLogic::getGridFromPath(const Path& path) {
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
    bool win = true;
    // Vérification des lignes
    for (int r = 0; r < rows; ++r) {
        auto first = getSubGrid(r, 0).currentShape;
        if (!isWinningShape(first)) continue;
        
        win = true;
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
        auto first = getSubGrid(0, c).currentShape;
        if (!isWinningShape(first)) continue;
        
        win = true;
        for (int r = 1; r < rows; ++r) {
            if (getSubGrid(r, c).currentShape != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    // Vérification de la diagonale principale
    auto first = getSubGrid(0, 0).currentShape;
    if (!isWinningShape(first)) {
        win = true;
        for (int i = 1; i < std::min(rows,cols); ++i) {
            if (getSubGrid(i, i).currentShape != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    // Vérification de l'anti-diagonale
    first = getSubGrid(0, cols-1).currentShape;
    if (!isWinningShape(first)) {
        win = true;
        for (int i = 1; i < std::min(rows,cols); ++i) {
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
            if (!getSubGrid(r, c).isLockedShaped()) {
                return GridShape::NONE;
            }
        }
    }

    return GridShape::DRAW;
}

bool GridLogic::playMove(const Path &path, GridShape player){
    return playMove(path, player, 0, false);
}

bool GridLogic::playMove(const Path &path, GridShape player, int step, bool locked, Path currentPath)
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
            setShape(checkVictory());
            return true;
        }
    }
    return false;
}

void GridLogic::undoMove(const Path &path, int step)
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
}

std::vector<Path> GridLogic::getAvailableMoves(const Path &target, Path currentPath) const
{
    std::vector<Path> result;

    bool locked = false;
    locked |= isLockedShaped();
    locked |= !starts_with(currentPath, target);
    if (locked) {
        return result;
    }

    if (isLeaf()) {
        result.push_back(currentPath);
    } else {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int index = r * cols + c;
                currentPath.push_back(index);
                auto tmp = getSubGrid(index).getAvailableMoves(target, currentPath);
                result.insert(result.end(), tmp.begin(), tmp.end());
                currentPath.pop_back();
            }
        }
    }
    return result;
}

std::optional<Path> GridLogic::getRandomAvailableMove(const Path& target, Path currentPath) const
{
    std::optional<Path> result = std::nullopt;

    bool locked = false;
    locked |= isLockedShaped();
    locked |= !starts_with(currentPath, target);
    if (locked) {
        return result;
    }

    if (isLeaf()) {
        return currentPath;
    } else {
        int random_start = rand() % (rows * cols);
        for(int i = random_start; i < random_start + (rows * cols); ++i){
            int index = i % (rows * cols);
            currentPath.push_back(index);
            auto tmp = getSubGrid(index).getRandomAvailableMove(target, currentPath);
            if(tmp){
                return tmp;
            }
            currentPath.pop_back();
        }
    }
    return result;
}