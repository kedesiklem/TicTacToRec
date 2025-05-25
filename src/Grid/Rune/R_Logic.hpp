#pragma once

#include "../GridLogic.hpp"

namespace Rune{

enum class Secteur {
    SERH,
    FAILLE,
};

class Rune{
    Secteur state;

    public:

    Rune(Secteur state = Secteur::SERH) : state(state) {}

    void toggle() {
        if (state == Secteur::SERH) {
            state = Secteur::FAILLE;
        } else {
            state = Secteur::SERH;
        }
    }

    friend class RuneLogic;
};

class RuneLogic : public GridLogic<Rune, RuneLogic> {
    public:
        RuneLogic(int rows, int cols, int depth = 0) : GridLogic<Rune, RuneLogic>(rows, cols, depth) {}

        Secteur getSecteur() const {
            return getCell().state;
        }

        bool isLocked() const {
            return getCell().state == Secteur::SERH;
        }

        bool toogle() {
            getCell().toggle();
            return true;
        }

        void toggleState(int row, int col) {
            if (row < 0 || row >= rows || col < 0 || col >= cols) {
                throw std::out_of_range("Invalid indices for toggling state");
            }
            RuneLogic& rune = getSubGrid(row, col);
            rune.getCell().toggle();
        }
};

}