#pragma once

#include "../GridView.hpp"
#include "R_Logic.hpp"

namespace Rune{

class RuneView : public GridView {

    ImVec2 lastMousePos = ImVec2(0, 0);

    public:

    using GridView::GridView;

    // On suppose que la cellule est survolé
    bool handleInteraction(RuneLogic& grid, const ImVec2& pos, const ImVec2& size) {
        ImGuiIO& io = ImGui::GetIO();
        if ((io.MouseDown[0] && !isHovered(lastMousePos, pos, size) || io.MouseClicked[0])) {
            return grid.toogle();
        }
    }

    bool drawLeafCell(ImDrawList* draw_list, RuneLogic& grid, const ImVec2& pos, const ImVec2& size) {

        bool interacted = false; 

        ImU32 bg_color = (grid.getSecteur() == Secteur::SERH) ? IM_COL32(10, 10, 10, 100) : 
                                                                IM_COL32(150, 150, 150, 255);
        draw_list->AddRectFilled(pos, pos + size, bg_color, 4.0f);

        if (isHovered(pos, size)) {

            interacted = handleInteraction(grid, pos, size);

            bg_color = IM_COL32(255, 255, 255, 50);  // Hovered white
            draw_list->AddRectFilled(pos, pos + size, bg_color, 4.0f);
        }

        return interacted;
    };

    void draw(RuneLogic& grid) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw(draw_list, grid, pos, size);
        lastMousePos = ImGui::GetMousePos();
    }

    void draw(ImDrawList* draw_list,  RuneLogic& grid, const ImVec2& pos, const ImVec2& size) {        
        std::pair<ImVec2, ImVec2> dim = calculateDimensions(size, grid.getCols(), grid.getRows(), ratio);
        if(grid.isLeaf()) {
            drawLeafCell(draw_list, grid, pos, size);
        } else {
            for (int r = 0; r < grid.getRows(); ++r) {
                for (int c = 0; c < grid.getCols(); ++c) {

                    ImVec2 cell_pos = calculateCellPosition(pos, dim, r, c);
                    draw(draw_list, grid.getSubGrid(r,c), cell_pos, dim.first);
                    
                }
            }
        }
    }


};

}