#pragma once

#include "../GridView.hpp"
#include "TTT_GridLogic.hpp"

namespace TTT {

    class TTT_GridView : public GridView{

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

    public : 

        TTT_GridView(float ratio = 0.8f)
            : GridView(ratio){}

        void draw(TTT_GridLogic& grid, Path targetSubGridPath = {}) {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw(draw_list, grid, pos, size, targetSubGridPath);
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

        std::optional<Path> handleGridInteraction(TTT_GridLogic& grid, Path targetSubGridPath = {}) {
            if(ImGui::IsMouseClicked(ImGuiMouseButton(0))) {
                return handleGridInteractionImpl(grid, pos, size, targetSubGridPath);
            } else {
                return std::nullopt;
            }
        }

    };

}