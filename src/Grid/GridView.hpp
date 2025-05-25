#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "../external/imgui/imgui.h"
#include "GridLogic.hpp"

class GridView {

protected:

    ImVec2 pos;
    ImVec2 size;

    bool isHovered(const ImVec2& pos, const ImVec2& size) {
        ImVec2 mousePos = ImGui::GetMousePos();
        return 
            ((mousePos.x > pos.x) && (mousePos.x < (pos.x + size.x))) &&
            ((mousePos.y > pos.y) && (mousePos.y < (pos.y + size.y)));
    }

public:
    float ratio;

    GridView(float ratio)
        : ratio(ratio) {}

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

    static ImVec2 calculateCellPosition(const ImVec2& base_pos, std::pair<ImVec2, ImVec2> dim, int r, int c) {
        return ImVec2(
            base_pos.x + c * (dim.first.x + dim.second.x),
            base_pos.y + r * (dim.first.y + dim.second.y)
        );
    }

    void update(const ImVec2& pos, const ImVec2& size) {
        this->pos = pos;
        this->size = size;
    }

};

