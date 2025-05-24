#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "../external/imgui/imgui.h"
#include "GridLogic.hpp"

class GridView {
    ImVec2 pos;
    ImVec2 size;

public:
    TTT_GridLogic& grid_root;
    float ratio;

    GridView(TTT_GridLogic& grid, float ratio)
        : grid_root(grid), ratio(ratio) {}

    static std::pair<ImVec2, ImVec2> calculateDimensions(const ImVec2& size, int cols, int rows, float ratio) {
        // Total padding space to distribute (based on ratio)
        float total_padding_x = size.x * (1.0f - ratio);
        float total_padding_y = size.y * (1.0f - ratio);
        
        // Distribute padding only between cells (not on outer edges)
        float padding_x = (cols > 1) ? total_padding_x / (cols - 1) : 0;
        float padding_y = (rows > 1) ? total_padding_y / (rows - 1) : 0;
        
        // Calculate cell size based on remaining space
        float cell_width = (size.x - (padding_x * (cols - 1))) / cols;
        float cell_height = (size.y - (padding_y * (rows - 1))) / rows;
        
        return {
            ImVec2(cell_width, cell_height), 
            ImVec2(padding_x, padding_y)
        };
    }

    ImVec2 calculateCellPosition(const ImVec2& base_pos, std::pair<ImVec2, ImVec2> dim, int r, int c) const {
        return ImVec2(
            base_pos.x + c * (dim.first.x + dim.second.x),
            base_pos.y + r * (dim.first.y + dim.second.y)
        );
    }

    void update(const ImVec2& pos, const ImVec2& size) {
        this->pos = pos;
        this->size = size;
    }

    void draw(Path targetSubGridPath = {}) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw(draw_list, grid_root, pos, size, targetSubGridPath);
    }

    void draw(ImDrawList* draw_list, const TTT_GridLogic& grid, const ImVec2& pos, const ImVec2& size, Path targetSubGridPath = {}, Path currentPath = {}, bool locked = false) {
        
        locked |= grid.isLocked();
        locked |= !starts_with(currentPath, targetSubGridPath);

        std::pair<ImVec2, ImVec2> dim = calculateDimensions(size, grid.getCols(), grid.getRows(), ratio);
        if(grid.isLeaf()) {
            drawLeafCell(draw_list, grid, pos, size, locked);
        } else {
            for (int r = 0; r < grid.getRows(); ++r) {
                for (int c = 0; c < grid.getCols(); ++c) {
                    currentPath.push_back(r * grid.getCols() + c);

                    ImVec2 cell_pos = calculateCellPosition(pos, dim, r, c);
                    draw(draw_list, grid.getSubGrid(r,c), cell_pos, dim.first, targetSubGridPath, currentPath, locked);
                    
                    currentPath.pop_back();
                }
            }
        }

        // Draw global shape if grid is won
        if (grid.isLocked()) {
            drawShape(draw_list, pos, size, grid.getShape());
        }
    }

    std::optional<Path> handleGridInteraction(Path targetSubGridPath = {}) {
        if(ImGui::IsMouseClicked(ImGuiMouseButton(0))) {
            return handleGridInteractionImpl(grid_root, pos, size, targetSubGridPath);
        } else {
            return std::nullopt;
        }
    }

private:
    std::optional<Path> handleGridInteractionImpl(const TTT_GridLogic& grid, const ImVec2& pos, const ImVec2& size, Path targetSubGridPath, Path currentPath = {}) {
        // Check if mouse is over this cell
        bool hovered = isHovered(pos, size);
        
        // If it's a leaf and we clicked on it
        if (grid.isLeaf() && hovered) {
            return currentPath;
        }
        
        // If not a leaf, traverse subgrids
        if (!grid.isLeaf()) {
            std::pair<ImVec2, ImVec2> dim = calculateDimensions(size, grid.getCols(), grid.getRows(), ratio);
            
            for (int r = 0; r < grid.getRows(); ++r) {
                for (int c = 0; c < grid.getCols(); ++c) {
                    currentPath.push_back(r * grid.getCols() + c);
                    
                    ImVec2 cell_pos = calculateCellPosition(pos, dim, r, c);
                    auto result = handleGridInteractionImpl(
                        grid.getSubGrid(r, c), 
                        cell_pos, 
                        dim.first,
                        targetSubGridPath, 
                        currentPath
                    );
                    
                    currentPath.pop_back();
                    
                    if (result.has_value()) {
                        return result;
                    }
                }
            }
        }
        
        return std::nullopt;
    }

    bool isHovered(const ImVec2& pos, const ImVec2& size) {
        ImVec2 mousePos = ImGui::GetMousePos();
        return 
            ((mousePos.x > pos.x) && (mousePos.x < (pos.x + size.x))) &&
            ((mousePos.y > pos.y) && (mousePos.y < (pos.y + size.y)));
    }

    void drawShape(ImDrawList* draw_list, const ImVec2& pos, const ImVec2& size, TTT_Shape shape) {
        switch(shape) {
            case TTT_Shape::CROSS:
                drawCross(draw_list, pos, size, size.x / 10);
                break;
            case TTT_Shape::CIRCLE:
                drawCircle(draw_list, pos, size, size.x / 10);
                break;
            case TTT_Shape::DRAW:
                drawDrawSymbol(draw_list, pos, size);
                break;
            default: break;
        }
    }

    void drawLeafCell(ImDrawList* draw_list, const TTT_GridLogic& grid, const ImVec2& pos, const ImVec2& size, bool locked) {
        if(locked) {
            ImU32 bg_color = (grid.getShape() == TTT_Shape::CROSS) ? IM_COL32(255, 0, 0, 50) : 
                            (grid.getShape() == TTT_Shape::CIRCLE) ? IM_COL32(0, 0, 255, 100) : 
                            IM_COL32(100, 100, 100, 255);  // Gray for locked
            draw_list->AddRectFilled(pos, pos + size, bg_color, 4.0f);
        } else {
            // Base background color
            ImU32 bg_color = IM_COL32(150, 150, 150, 255);  // Base gray
            
            // Override if hovered
            if (isHovered(pos, size)) {
                bg_color = IM_COL32(255, 255, 255, 255);  // Hovered white
            }

            draw_list->AddRectFilled(pos, pos + size, bg_color, 4.0f);
        }
    }

    // Drawing helpers
    void drawCross(ImDrawList* draw_list, const ImVec2& pos, const ImVec2 size, float thickness = 10.0f) {
        float margin_x = size.x * 0.2f;
        float margin_y = size.y * 0.2f;
        draw_list->AddLine(
            ImVec2(pos.x + margin_x, pos.y + margin_y),
            ImVec2(pos.x + size.x - margin_x, pos.y + size.y - margin_y),
            IM_COL32(255, 0, 0, 255), thickness);
        draw_list->AddLine(
            ImVec2(pos.x + margin_x, pos.y + size.y - margin_y),
            ImVec2(pos.x + size.x - margin_x, pos.y + margin_y),
            IM_COL32(255, 0, 0, 255), thickness);
    }

    void drawCircle(ImDrawList* draw_list, const ImVec2& pos, const ImVec2 size, float thickness = 3.0f) {
        float margin_x = size.x * 0.15f;
        float margin_y = size.x * 0.15f;
        ImVec2 center = {pos.x + size.x/2, pos.y + size.y/2};
        ImVec2 radius = {(size.x - margin_x*2)/2, (size.y - margin_y*2)/2};
        draw_list->AddEllipse(center, radius, IM_COL32(0, 0, 255, 255), 0, 0, thickness);
    }

    void drawDrawSymbol(ImDrawList* draw_list, const ImVec2& pos, const ImVec2 size) {
        ImU32 bg_color = IM_COL32(10, 10, 10, 150);
        draw_list->AddRectFilled(pos, pos + size, bg_color, 4.0f);
    }
};