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
        return ((io.MouseDown[0] && !isHovered(lastMousePos, pos, size) || io.MouseClicked[0]));
    }

    std::optional<Path> drawLeafCell(ImDrawList* draw_list, RuneLogic& grid, const ImVec2& pos, const ImVec2& size, Path path) {

        std::optional<Path> interacted = std::nullopt; 

        ImU32 bg_color = (grid.getSecteur() == Secteur::SERH) ? IM_COL32(150, 150, 150, 100) : 
                                                                IM_COL32(10, 10, 10, 100);
        draw_list->AddRectFilled(pos, pos + size, bg_color, 4.0f);

        if (isHovered(pos, size)) {

            if(handleInteraction(grid, pos, size)){
                interacted = path;
            }

            bg_color = IM_COL32(255, 255, 255, 50);  // Hovered white
            draw_list->AddRectFilled(pos, pos + size, bg_color, 4.0f);
        }

        return interacted;
    };

    std::optional<Path> draw(RuneLogic& grid) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        auto path = draw(draw_list, grid, pos, size);
        lastMousePos = ImGui::GetMousePos();
        return path;
    }

    std::optional<Path> draw(ImDrawList* draw_list,  RuneLogic& grid, const ImVec2& pos, const ImVec2& size, Path path = {}) {   
        std::optional<Path> interacted = std::nullopt;      
        std::pair<ImVec2, ImVec2> dim = calculateDimensions(size, grid.getCols(), grid.getRows(), ratio);
        if(grid.isLeaf()) {
            return drawLeafCell(draw_list, grid, pos, size, path);
        } else {
            if(grid.getSecteur() == Secteur::SERH) {
                draw_list->AddRectFilled(pos, pos + size, IM_COL32(0, 0, 255, 50), 4.0f);

            } else {
                draw_list->AddRectFilled(pos, pos + size, IM_COL32(255, 0, 0, 50), 4.0f);
            }
            for (int r = 0; r < grid.getRows(); ++r) {
                for (int c = 0; c < grid.getCols(); ++c) {
                    path.push_back(r * grid.getCols() + c);

                    ImVec2 cell_pos = calculateCellPosition(pos, dim, r, c);
                    auto tmp = draw(draw_list, grid.getSubGrid(r,c), cell_pos, dim.first, path);
                    if(tmp) {
                            interacted = tmp;
                    }

                    path.pop_back();

                }
            }
        }
        return interacted;
    }
};

}