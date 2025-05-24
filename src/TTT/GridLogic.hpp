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

enum class TTT_Shape {
    NONE,
    CROSS,
    CIRCLE,
    DRAW,
};

class TTT_Cell{
    TTT_Shape currentShape = TTT_Shape::NONE;
    public :

        operator TTT_Shape() const {
            return currentShape;
        }

        TTT_Cell operator= (TTT_Shape shape) {
            currentShape = shape;
            return currentShape;
        }

        TTT_Cell() = default;
        TTT_Cell(TTT_Shape shape) : currentShape(shape) {}

        static TTT_Shape nextShapePlayable(const TTT_Shape shape);
        static std::string ShapeToString(const TTT_Shape shape);
        static TTT_Shape StringToShape(const std::string& str);
        static bool isWinning(const TTT_Shape shape){
            return (shape == TTT_Shape::CROSS || shape == TTT_Shape::CIRCLE);
        }
        static bool isLocked(const TTT_Shape shape){
            return shape != TTT_Shape::NONE;
        }
        
        TTT_Shape nextShapePlayable() const {
            return nextShapePlayable(currentShape);
        }
        std::string ShapeToString() const {
            return ShapeToString(currentShape);
        }
        bool isWinning() const{
            return isWinning(currentShape);
        }
        bool isLocked() const{
            return isLocked(currentShape);
        }
};

inline std::ostream& operator<<(std::ostream& os, const TTT_Shape& shape){
    os << TTT_Cell::ShapeToString(shape);
    return os;
};

template<typename T, typename D>
class GridLogic {
    protected:

        int rows;
        int cols;
        T cell = T();
        std::vector<std::vector<D>> subGrids;

    public:

        GridLogic(int rows, int cols, int depth) : rows(rows), cols(cols), cell(T()) {
            if(rows <= 0 || cols <= 0) {
                throw std::invalid_argument("Rows and columns must be greater than zero.");
            }
            if (depth < 0) {
                throw std::invalid_argument("Depth must be non-negative.");
            }
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
        };

        void resetGrid() {
            cell = T();
            for (auto& row : subGrids) {
                for (auto& subGrid : row) {
                    subGrid.resetGrid();
                }
            }
        };

        void setCell(const T& value) {
            cell = value;
        }
        const T& getCell() const {
            return cell;
        }
        T& getCell() {
            return cell;
        }

        bool isLeaf() const { 
            return subGrids.empty(); 
        }

        int getRows() const {
            return rows;
        } 
        int getCols() const {
            return cols;
        } 

        D& getSubGrid(int row, int col) {
            if (row >= 0 && row < rows && col >= 0 && col < cols) {
                return subGrids[row][col];
            }
            throw std::out_of_range("Invalid subgrid indices");
        };
        const D& getSubGrid(int row, int col) const {
            if (row >= 0 && row < rows && col >= 0 && col < cols) {
                return subGrids[row][col];
            }
            throw std::out_of_range("Invalid subgrid indices [const]");
        };

        D &getSubGrid(int index){
            if (index < 0 || index >= rows * cols) {
                throw std::out_of_range("Invalid subgrid indices [const]");
            }
        
            int r = index / cols;
            int c = index % cols;
        
            return getSubGrid(r,c);
        };
        const D &getSubGrid(int index) const{
            if (index < 0 || index >= rows * cols) {
                throw std::out_of_range("Invalid subgrid indices [const]");
            }

            int r = index / cols;
            int c = index % cols;

            return getSubGrid(r,c);
        };

        const D& getGridFromPath(const Path& path) const {
            const GridLogic* grid = this;
            for (int index : path) {
                if (index < 0 || index >= grid->rows * grid->cols) {
                    throw std::out_of_range("Invalid subgrid indices in path");
                }
                if (grid->isLeaf()) {
                    throw std::runtime_error("Cannot navigate deeper into a leaf grid");
                }
                int row = index / grid->cols;
                int col = index % grid->cols;
                grid = &grid->subGrids[row][col];
            }
            return *static_cast<const D*>(grid);
        }
        
        D& getGridFromPath(const Path& path) {
            GridLogic* grid = this;
            for (int index : path) {
                if (index < 0 || index >= grid->rows * grid->cols) {
                    throw std::out_of_range("Invalid subgrid indices in path");
                }
                if (grid->isLeaf()) {
                    throw std::runtime_error("Cannot navigate deeper into a leaf grid");
                }
                int row = index / grid->cols;
                int col = index % grid->cols;
                grid = &grid->subGrids[row][col];
            }
            return *static_cast<D*>(grid);
        }
        template<typename U, typename V>
        friend std::ostream& operator<<(std::ostream& os, const GridLogic<U, V>& grid);
    };


class TTT_GridLogic : public GridLogic<TTT_Cell, TTT_GridLogic> {
    public:
        TTT_GridLogic(int rows, int cols, int depth = 0) : GridLogic<TTT_Cell, TTT_GridLogic>(rows, cols, depth) {}
        TTT_Shape checkVictory();

        const TTT_Shape getShape() const {
            return cell;
        };

        bool isWinning() const {
            return cell.isWinning();
        }
        bool isLocked() const {
            return (cell.isLocked());
        }

        void setShape(const TTT_Shape shape) {
            cell = shape;
        }

        bool playMove(const Path& path, TTT_Shape player);
        bool playMove(const Path& path, TTT_Shape player, int step, bool locked, Path currentPath = {});
        void undoMove(const Path& path, int step = 0);

        std::vector<Path> getAvailableMoves(const Path& target = {}, Path currentPath = {}) const;
        std::optional<Path> getRandomAvailableMove(const Path& target = {}, Path currentPath = {}) const;
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
inline std::ostream& operator<<(std::ostream& os, const TTT_Cell& cell){ 
    os << cell.ShapeToString();
    return os;
};
template<typename T, typename D>
inline std::ostream& operator<<(std::ostream& os, const GridLogic<T, D>& grid){
    os << grid.currentShape;
    if(!grid.subGrids.empty())
        os << grid.subGrids;
    return os;
};
