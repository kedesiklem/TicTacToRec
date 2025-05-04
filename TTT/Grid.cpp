#include "Grid.h"
#include <iostream>
#include <algorithm>

std::ostream& operator<<(std::ostream& os, const GridShape& shape){
    switch (shape) {
        case GridShape::NONE: os << "NONE"; break;
        case GridShape::CROSS: os << "CROSS"; break;
        case GridShape::CIRCLE: os << "CIRCLE"; break;
        case GridShape::DRAW: os << "DRAW"; break;
    }
    return os;
};

Grid::Grid(int rows, int cols, int rec, float cellSize, float padding) 
    : rows(rows), cols(cols), cellSize(std::max(cellSize, GRID_MIN_SIZE)), padding(std::max(padding, GRID_MIN_PADDING)) {
    
    if (rows > 1 || cols > 1) {
        subGrids.resize(rows);
        for (int r = 0; r < rows; ++r) {
            subGrids[r].reserve(cols);
            for (int c = 0; c < cols; ++c) {
                if(rec > 0) {
                    subGrids[r].emplace_back(rows, cols, rec - 1, cellSize, padding / 2);
                } else {
                    subGrids[r].emplace_back(1, 1, cellSize, padding);
                }
            }
        }
    }
}

void Grid::setPosition(float x, float y) {
    startX = x;
    startY = y;
}

void Grid::setWindowSize(float width, float height) {
    cellSize = (std::min(width, height) - (cols - 1) * padding) / cols;
    if (cellSize < GRID_MIN_SIZE) cellSize = GRID_MIN_SIZE;
    
    for (auto& row : subGrids) {
        for (auto& subGrid : row) {
            subGrid.setWindowSize(cellSize, cellSize);
        }
    }
}

void Grid::setShape(GridShape shape) {
    currentShape = shape;
}

void Grid::drawv2(const ImVec2& window_pos, const std::vector<int> targetSubGridPath, std::vector<int> currentPath, int recursionLevel){
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    if(isLeaf()){
        ImU32 color = hovered ? IM_COL32(200, 200, 200, 255) : 
                                IM_COL32(150, 150, 150, 255);

        color = (currentShape == GridShape::CROSS) ? IM_COL32(255, 0, 0, 155) : 
                (currentShape == GridShape::CIRCLE) ? IM_COL32(0, 0, 255, 155) : 
                color;

        draw_list->AddRectFilled(
            ImVec2(window_pos.x, window_pos.y), 
            ImVec2(window_pos.x + cellSize, window_pos.y + cellSize), 
            color, 
            0.0f
        );

        draw_list->AddRect(
            ImVec2(window_pos.x, window_pos.y), 
            ImVec2(window_pos.x + cellSize, window_pos.y + cellSize), 
            IM_COL32(50, 50, 50, 255), 
            0.0f, 
            0, 
            1.0f
        );
    } else {
        for(int r = 0; r < getRows(); ++r){
            for(int c = 0; c < getCols(); ++c){


                bool lock = false;
                if(!targetSubGridPath.empty() && recursionLevel < targetSubGridPath.size()){
                    int index = targetSubGridPath[recursionLevel];
                    int target_r = index / getCols();
                    int target_c = index % getCols();
                    if(r != target_r || c != target_c){
                        lock = true;
                    }
                 } 

                 float x = window_pos.x + c * (cellSize + padding);
                 float y = window_pos.y + r * (cellSize + padding);
                 if(lock ||isLockedShaped()){
                    subGrids[r][c].drawv2(ImVec2(x, y), {}, currentPath, recursionLevel + 1);
                    float x = window_pos.x + c * (cellSize + padding);
                    float y = window_pos.y + r * (cellSize + padding);
                    draw_list->AddRectFilled(
                        ImVec2(x, y), 
                        ImVec2(x + cellSize, y + cellSize), 
                        IM_COL32(0, 0, 0, 100), // Gris semi-transparent
                        0.0f
                    );
                 }else{
                    subGrids[r][c].drawv2(ImVec2(x, y), targetSubGridPath, currentPath, recursionLevel + 1);
                 }
                }
            }
    }
    if (currentShape != GridShape::NONE) {
        ImVec2 grid_start = window_pos;
        ImVec2 grid_end = ImVec2(
            window_pos.x + cols * (cellSize + padding) - padding,
            window_pos.y + rows * (cellSize + padding) - padding
        );

        float width = (grid_end.x - grid_start.x) / 20;
        switch(currentShape) {
            case GridShape::CROSS:
                draw_list->AddLine(
                    ImVec2(grid_start.x + width, grid_start.y + width),
                    ImVec2(grid_end.x - width, grid_end.y - width),
                    IM_COL32(255, 0, 0, 255), width);
                draw_list->AddLine(
                    ImVec2(grid_start.x + width, grid_end.y - width),
                    ImVec2(grid_end.x - width, grid_start.y + width),
                    IM_COL32(255, 0, 0, 255), width);
                break;
                
            case GridShape::CIRCLE:
                draw_list->AddCircle(
                    ImVec2((grid_start.x + grid_end.x) * 0.5f, (grid_start.y + grid_end.y) * 0.5f),
                    (grid_end.x - grid_start.x) * 0.45f,
                    IM_COL32(0, 0, 255, 255),
                    0, width);
                break;
        }
    }
}

Grid& Grid::getSubGrid(int row, int col) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return subGrids[row][col];
    }
    throw std::out_of_range("Invalid subgrid indices");
}

const Grid& Grid::getSubGrid(int row, int col) const {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return subGrids[row][col];
    }
    throw std::out_of_range("Invalid subgrid indices");
};

void Grid::resetGrid() {
    currentShape = GridShape::NONE;
    clicked = false;
    hovered = false;
    for (auto& row : subGrids) {
        for (auto& subGrid : row) {
            subGrid.resetGrid();
        }
    }
}

GridShape Grid::checkVictory(){
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