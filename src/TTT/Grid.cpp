#include "Grid.h"
#include <iostream>
#include <algorithm>


GridShape nextShapePlayable(GridShape shape){
    switch (shape){
        case GridShape::CROSS:
            return GridShape::CIRCLE;
        
        case GridShape::CIRCLE:
            return GridShape::CROSS;

        default:
            return shape;
    }
}
std::string GridShapeToString(GridShape shape){
    switch (shape)
    {
    case GridShape::NONE :
        return "None";
        break;
    
    case GridShape::CROSS :
        return "X";
        break;
    
    case GridShape::CIRCLE :
        return "O";
        break;
        
    case GridShape::DRAW :
        return "-";
        break;
    
    default:
        return "Undefined";
        break;
    }
}

template <typename T>
bool starts_with(const std::vector<T>& vec, const std::vector<T>& prefix) {
    // Si le préfixe est plus grand que le vecteur, il ne peut pas être au début
    if (prefix.size() > vec.size()) {
        return false;
    }
    
    // Compare les premiers éléments du vecteur avec le préfixe
    return std::equal(prefix.begin(), prefix.end(), vec.begin());
}

std::ostream& operator<<(std::ostream& os, const GridShape& shape){
    switch (shape) {
        case GridShape::NONE: os << "NONE"; break;
        case GridShape::CROSS: os << "CROSS"; break;
        case GridShape::CIRCLE: os << "CIRCLE"; break;
        case GridShape::DRAW: os << "DRAW"; break;
    }
    return os;
};

Grid Grid::getGridFromPath(const std::vector<int>& path) {
    Grid* grid = this;
    for (int index : path) {
        int row = index / cols;
        int col = index % cols;
        grid = &grid->subGrids[row][col];
    }
    return *grid;
}

Grid::Grid(int rows, int cols, int rec, float cellSize, float padding) 
    : rows(rows), cols(cols), cellSize(std::max(cellSize, GRID_MIN_SIZE)), padding(std::max(padding, GRID_MIN_PADDING)) {
    
    if (rows > 1 || cols > 1) {
        subGrids.resize(rows);
        for (int r = 0; r < rows; ++r) {
            subGrids[r].reserve(cols);
            for (int c = 0; c < cols; ++c) {
                if(rec > 0) {
                    subGrids[r].emplace_back(rows, cols, rec - 1, cellSize, padding / 1.5f);
                } else {
                    subGrids[r].emplace_back(1, 1, 0, cellSize, padding);
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

void Grid::drawCross(ImDrawList* draw_list, const ImVec2& start, const ImVec2& end, float width, ImU32 color) {
    int thickness = 1;
    draw_list->AddLine(
        ImVec2(start.x + width, start.y + width),
        ImVec2(end.x - width, end.y - width),
        color, thickness * width);
    draw_list->AddLine(
        ImVec2(start.x + width, end.y - width),
        ImVec2(end.x - width, start.y + width),
        color, thickness * width);
}

void Grid::drawCircle(ImDrawList* draw_list, const ImVec2& start, const ImVec2& end, float width, ImU32 color) {
    ImVec2 s = ImVec2(start.x + width/2, start.y + width/2); 
    ImVec2 e = ImVec2(end.x - width/2, end.y - width/2); 
    
    int thickness = 1;
    draw_list->AddCircle(
        ImVec2((s.x + e.x) * 0.5f, (s.y + e.y) * 0.5f),
        (e.x - s.x) * 0.45f,
        color,
        0, thickness * width);
}

void Grid::drawShape(ImDrawList* draw_list, const ImVec2& start, const ImVec2& end, 
    float width, ImU32 cross_color, 
    ImU32 circle_color) {
    switch(currentShape) {
    case GridShape::CROSS:
        drawCross(draw_list, start, end, width, cross_color);
        break;
        
    case GridShape::CIRCLE:
        drawCircle(draw_list, start, end, width, circle_color);
        break;
        
    case GridShape::DRAW:
        break;
        
    case GridShape::NONE:
    default:
        break;
    }
}

void Grid::draw(const ImVec2& window_pos, const std::vector<int> targetSubGridPath, std::vector<int> currentPath, int recursionLevel, bool locked){
    
    locked |= currentShape != GridShape::NONE;

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
            cellSize/10
        );

        if(!starts_with(currentPath, targetSubGridPath) || locked)
            draw_list->AddRectFilled(
                window_pos, 
                ImVec2(window_pos.x + cellSize, window_pos.y + cellSize), 
                IM_COL32(0, 0, 0, 100), // Gris semi-transparent
                cellSize/10
            );

    } else {
        for(int r = 0; r < getRows(); ++r){
            for(int c = 0; c < getCols(); ++c){

                float x = window_pos.x + c * (cellSize + padding);
                float y = window_pos.y + r * (cellSize + padding);
                currentPath.push_back(r * getCols() + c);
                subGrids[r][c].draw(
                    ImVec2(x, y), 
                    targetSubGridPath, 
                    currentPath, 
                    recursionLevel + 1,
                    locked
                );
                currentPath.pop_back();          
            }
        }
    }

    // Affiche Shape
    if (currentShape != GridShape::NONE) {
        ImVec2 grid_start = window_pos;
        ImVec2 grid_end = ImVec2(
            window_pos.x + cols * (cellSize + padding) - padding,
            window_pos.y + rows * (cellSize + padding) - padding
        );

        float width = (grid_end.x - grid_start.x) / 10;

        ImU32 color =   (currentShape == GridShape::CROSS) ? IM_COL32(255, 0, 0, 155) :  IM_COL32(0, 0, 255, 155);

        drawShape(draw_list,grid_start,grid_end, width);
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