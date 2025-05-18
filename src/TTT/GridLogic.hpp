#pragma once

#include <vector>
#include <iostream>

template <typename T>
bool starts_with(const std::vector<T>& vec, const std::vector<T>& prefix) {
    if (prefix.size() > vec.size()) return true;
    
    return std::equal(prefix.begin(), prefix.end(), vec.begin());
}

enum class GridShape {
    NONE,
    CROSS,
    CIRCLE,
    DRAW,
};

std::ostream& operator<<(std::ostream& os, const GridShape& shape);

class GridLogic {
    int rows;
    int cols;
    GridShape currentShape = GridShape::NONE;
    std::vector<std::vector<GridLogic>> subGrids;

    public:

    GridLogic(int rows, int cols, int depth = 0);

    void resetGrid();

    static GridShape nextShapePlayable(const GridShape shape);
    static std::string GridShapeToString(const GridShape shape);
    static GridShape StringToGridShape(const std::string& str);
    static bool isWinningShape(const GridShape shape) {
        return (shape == GridShape::CROSS || shape == GridShape::CIRCLE);
    }

    bool isWinningShape() const {
        return isWinningShape(currentShape);
    }
    bool isLockedShaped() const {
        return (currentShape != GridShape::NONE);
    }
    bool isLeaf() const { 
        return subGrids.empty(); 
    }
    const GridShape getShape() const {
        return currentShape;
    };
    void setShape(const GridShape shape) {
        currentShape = shape;
    }
    int getRows() const {
        return rows;
    } 
    int getCols() const {
        return cols;
    } 

    GridLogic& getSubGrid(int index);
    GridLogic &getSubGrid(size_t row, size_t col);
    const GridLogic &getSubGrid(size_t row, size_t col) const;
    GridLogic getGridFromPath(const std::vector<int>& path);
    GridShape checkVictory();

    std::vector<std::vector<int>> getValidMoves(std::vector<int> target);
    bool playMove(const std::vector<int>& path, GridShape player);
    bool playMove(const std::vector<int>& path, GridShape player, size_t step, bool locked, std::vector<int> currentPath = {});

    void undoMove(const std::vector<int>& path, size_t step = 0);

    friend std::ostream& operator<<(std::ostream& os, const GridShape& shape);
};

std::ostream& operator<<(std::ostream& os, const GridShape& shape);