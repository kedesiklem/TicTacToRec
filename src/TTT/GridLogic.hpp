#pragma once

#include <vector>
#include <iostream>
#include <cmath>
#include <optional>

template <typename T>
bool starts_with(const std::vector<T>& vec, const std::vector<T>& prefix) {
    if (prefix.size() > vec.size()) return true;
    
    return std::equal(prefix.begin(), prefix.end(), vec.begin());
}

using Path = std::vector<int>;

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
        const GridLogic& getSubGrid(int index) const;
        GridLogic &getSubGrid(int row, int col);
        const GridLogic &getSubGrid(int row, int col) const;
        GridLogic getGridFromPath(const Path& path);
        GridShape checkVictory();

        bool playMove(const Path& path, GridShape player);
        bool playMove(const Path& path, GridShape player, int step, bool locked, Path currentPath = {});

        void undoMove(const Path& path, int step = 0);

        std::vector<Path> getAvailableMoves(const Path& target = {}, Path currentPath = {}) const;
        std::optional<Path> getRandomAvailableMove(const Path& target = {}, Path currentPath = {}) const;

        friend std::ostream& operator<<(std::ostream& os, const GridLogic& grid);
    };

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v){
    os << "[";
    for(size_t i=0; i<v.size(); ++i){
            if (i != 0) os << ";";
            os << v[i];
    }
    os << "]";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const GridShape& shape){ 
    os << GridLogic::GridShapeToString(shape);
    return os;
};

inline std::ostream& operator<<(std::ostream& os, const GridLogic& grid){
    os << grid.currentShape;
    if(!grid.subGrids.empty())
        os << grid.subGrids;
    return os;
};
