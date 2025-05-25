#pragma once

template<typename V, typename G>
class ModeState{
    public :
        G& grid;
        V& view;

        virtual void reset() = 0;
        virtual void showParam() = 0;
        ModeState(V& view, G& grid) : view(view), grid(grid){};
};