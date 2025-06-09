#pragma once

template<typename G, typename V>
class ModeState{
    public :
        G& grid;
        V& view;

        G& getGrid(){return grid;};
        V& getView(){return view;};

        virtual void reset() = 0;
        virtual void showParam() = 0;
        ModeState(G& grid, V& view) : grid(grid), view(view){};
};