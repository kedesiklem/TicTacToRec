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
template<typename T, typename D>
inline std::ostream& operator<<(std::ostream& os, const GridLogic<T, D>& grid){
    os << grid.currentShape;
    if(!grid.subGrids.empty())
        os << grid.subGrids;
    return os;
};