#include "GridLogic.hpp"



TTT_Shape TTT_GridLogic::checkVictory(){

    if(isLeaf()){
        return cell;
    }
    bool win = true;
    // Vérification des lignes
    for (int r = 0; r < rows; ++r) {
        TTT_Shape first = getSubGrid(r, 0).getCell();
        if (!TTT_Cell::isWinning(first)) continue;
        
        win = true;
        for (int c = 1; c < cols; ++c) {
            if (getSubGrid(r, c).getCell() != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    // Vérification des colonnes
    for (int c = 0; c < cols; ++c) {
        auto first = getSubGrid(0, c).getCell();
        if (!TTT_Cell::isWinning(first)) continue;
        
        win = true;
        for (int r = 1; r < rows; ++r) {
            if (getSubGrid(r, c).getCell() != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    // Si grille carré verification des diagonal
    if(cols == rows){
        // Vérification de la diagonale principale
        auto first = getSubGrid(0, 0).getCell();
        if (TTT_Cell::isWinning(first)) {
            win = true;
            for (int i = 1; i < rows; ++i) {
                if (getSubGrid(i, i).getCell() != first) {
                    win = false;
                    break;
                }
            }
            if (win) return first;
        }
        
        

        // Vérification de l'anti-diagonale
        first = getSubGrid(0, cols-1).getCell();
        if (TTT_Cell::isWinning(first)) {
            win = true;
            for (int i = 1; i < rows; ++i) {
                if (getSubGrid(i, cols-1-i).getCell() != first) {
                    win = false;
                    break;
                }
            }
            if (win) return first;
        }
    }

    // Vérification du match nul
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (!getSubGrid(r, c).isLocked()) {
                return TTT_Shape::NONE;
            }
        }
    }

    return TTT_Shape::DRAW;
}

bool TTT_GridLogic::playMove(const Path &path, TTT_Shape player){
    return playMove(path, player, 0, false);
}

bool TTT_GridLogic::playMove(const Path &path, TTT_Shape player, int step, bool locked, Path currentPath)
{
    locked |= isLocked();
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

void TTT_GridLogic::undoMove(const Path &path, int step) {
    if(step == path.size()) {
        if(isLeaf() && isLocked()) {
            setShape(TTT_Shape::NONE);
        } else {
            throw std::runtime_error("Invalid Path");
        }
    } else {
        setShape(TTT_Shape::NONE);
        getSubGrid(path[step]).undoMove(path, step + 1); // Plus besoin de cast
    }
}

TTT_Shape TTT_Cell::nextShapePlayable(const TTT_Shape shape){
    switch (shape){
        case TTT_Shape::CROSS:
            return TTT_Shape::CIRCLE;
        
        case TTT_Shape::CIRCLE:
            return TTT_Shape::CROSS;

        default:
            return shape;
    }
}

std::string TTT_Cell::ShapeToString(TTT_Shape shape) {
    switch(shape) {
        case TTT_Shape::NONE: return "N";
        case TTT_Shape::CROSS: return "X";
        case TTT_Shape::CIRCLE: return "O";
        case TTT_Shape::DRAW: return "=";
        default: return "?";
    }
}

TTT_Shape TTT_Cell::StringToShape(const std::string& str) {
    if (str == "N") return TTT_Shape::NONE;
    if (str == "X") return TTT_Shape::CROSS;
    if (str == "O") return TTT_Shape::CIRCLE;
    if (str == "=") return TTT_Shape::DRAW;
    return TTT_Shape::NONE; // Valeur par défaut
}

std::vector<Path> TTT_GridLogic::getAvailableMoves(const Path &target, Path currentPath) const {
    std::vector<Path> result;
    bool locked = isLocked() || !starts_with(currentPath, target);
    
    if (locked) return result;

    if (isLeaf()) {
        result.push_back(currentPath);
    } else {
        for (int r = 0; r < getRows(); ++r) {
            for (int c = 0; c < getCols(); ++c) {
                int index = r * getCols() + c;
                currentPath.push_back(index);
                auto subMoves = getSubGrid(index).getAvailableMoves(target, currentPath);
                result.insert(result.end(), subMoves.begin(), subMoves.end());
                currentPath.pop_back();
            }
        }
    }
    return result;
}

std::optional<Path> TTT_GridLogic::getRandomAvailableMove(const Path& target, Path currentPath) const {
    auto allMoves = getAvailableMoves(target, currentPath);
    if (allMoves.empty()) return std::nullopt;
    
    int randomIndex = rand() % allMoves.size();
    return allMoves[randomIndex];
}