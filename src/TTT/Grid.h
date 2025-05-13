#ifndef GRID_H
#define GRID_H

#include <vector>
#include "imgui.h"
#include <iostream>

#define GRID_MIN_SIZE 10.0f
#define GRID_MIN_PADDING 2.0f

enum class GridShape {
    NONE,
    CROSS,
    CIRCLE,
    DRAW,
};

GridShape nextShapePlayable(GridShape shape);
std::string GridShapeToString(GridShape shape);

std::ostream& operator<<(std::ostream& os, const GridShape& shape);


class Grid {
    private:
        float cellSize;
        float padding;
        float startX;
        float startY;
        int rows;
        int cols;
        GridShape currentShape = GridShape::NONE;
        bool hovered = false;
        bool clicked = false;
        std::vector<std::vector<Grid>> subGrids;
        
    public:
        Grid(int rows = 1, int cols = 1, int rec = 0, float cellSize = 50.0f, float padding = 2.0f);
        
        //getters et setters pour hoverd clicked
        void toggleClicked() { clicked = !clicked; }
        
        bool isHovered() const { return hovered; }
        void setHovered(bool value) { hovered = value; }

        bool isClicked() const { return clicked; }
        void setClicked(bool value) { clicked = value; }
        
        
        void setWindowSize(float width, float height);
        void setPosition(float x, float y);
        void setShape(GridShape shape);
        
        const GridShape getShape() const { return currentShape; }
        int getRows() const { return rows; }
        int getCols() const { return cols; }
        float getTotalWidth() const { return cols * (cellSize + padding) - padding; }
        float getTotalHeight() const { return rows * (cellSize + padding) - padding; }
        float getCellSize() const { return cellSize; }
        float getPadding() const { return padding; }
        Grid& getSubGrid(int row, int col);
        const Grid& getSubGrid(int row, int col) const;
        
        bool isLeaf() const { return subGrids.empty(); }
        
        void draw(const ImVec2& window_pos, const std::vector<int> targetSubGridPath = {}, std::vector<int> currentPath = {}, int recursionLevel = 0, bool locked = false);
        
        void resetGrid();
        
        bool isWinningShape(GridShape shape) const {
            return (shape == GridShape::CROSS || shape == GridShape::CIRCLE);
        }

        bool isLockedShaped() const {
            return (currentShape != GridShape::NONE);
        }

        Grid getGridFromPath(const std::vector<int>& path);

        GridShape checkVictory();

        void drawCross(ImDrawList* draw_list, const ImVec2& start, const ImVec2& end, float width, ImU32 color);

        void drawCircle(ImDrawList* draw_list, const ImVec2& start, const ImVec2& end, float width, ImU32 color);

        void drawShape(ImDrawList* draw_list, const ImVec2& start, const ImVec2& end, 
            float width = 2.0f, ImU32 cross_color = IM_COL32(255, 0, 0, 255), 
            ImU32 circle_color = IM_COL32(50, 50, 255, 255));

        std::vector<std::vector<int>> getValidMoves(const std::vector<int>& currentPath, const std::vector<int>& targetPath) const;
        std::vector<std::vector<int>> getValidMoves(const std::vector<int>& targetPath) const;

        };


#endif // GRID_H