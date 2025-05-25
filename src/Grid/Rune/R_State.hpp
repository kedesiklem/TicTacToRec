#pragma once

#include "../ModeState.hpp"
#include "R_View.hpp"

namespace Rune{

class RuneState : public ModeState<RuneView, RuneLogic> {
public:
    using ModeState<RuneView, RuneLogic>::ModeState;

    void reset() override {
        grid.resetGrid();
    }

    void showParam() override {
        ImGui::Text("Rows: %d, Cols: %d", grid.getRows(), grid.getCols());
    }

    void draw(){
        auto path = view.draw(grid);
        if(path) {
            grid.toggleRune(path.value());
            grid.updateSecteur();
        }
    }
};

}