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

    Rune(Secteur state = Secteur::FAILLE) : state(state) {}

    Secteur toggle() {
        if (state == Secteur::SERH) {
            state = Secteur::FAILLE;
        } else {
            state = Secteur::SERH;
        }
        return state;
    }

    Secteur getState() const {
        return state;
    }

    void setState(Secteur newSecteur) {
        state = newSecteur;
    }

    friend class RuneLogic;
};

class RuneLogic : public GridLogic<Rune, RuneLogic> {
    public:
        RuneLogic(int rows, int cols, int depth = 0) : GridLogic<Rune, RuneLogic>(rows, cols, depth) {}

        Secteur getSecteur() const {
            return getCell().state;
        }

        bool isSerh() const {
            return getSecteur() == Secteur::SERH;
        }

        Secteur updateSecteur(){
            if(!isLeaf()){
                cell.setState(Secteur::FAILLE);
                auto sect = Secteur::FAILLE;
                for (int r = 0; r < rows; ++r) {
                    for (int c = 0; c < cols; ++c) {
                        sect = getSubGrid(r, c).updateSecteur();
                        if(sect == Secteur::SERH){
                            cell.setState(Secteur::SERH);
                        }
                    }
                }
            }
            return getSecteur();
        }

        bool isLocked(int index) const {

            int corner[4] = {0, 0, 0, 0};

            int r = index / cols;
            int c = index % cols;
            for(int offsetR = -1; offsetR <= 1; ++offsetR) {
                for(int offsetC = -1; offsetC <= 1; ++offsetC) {
                    if ((offsetR == 0 && offsetC == 0) 
                        || ((r+offsetR < 0) || (r+offsetR >= rows) 
                        || (c+offsetC) < 0 || (c+offsetC) >= cols )) 
                        continue; // Skip the center cell and out of bounds

                    if (getSubGrid(r + offsetR,c + offsetC).isSerh()){
                        // consider the 3 cells around the current cell for each corner
                        if(!(offsetR == -1 || offsetC == -1)) {
                            corner[0]++;
                        } if(!(offsetR == -1 || offsetC == 1)) {
                            corner[1]++;
                        } if(!(offsetR == 1 || offsetC == -1)) {
                            corner[2]++;
                        } if(!(offsetR == 1 || offsetC == 1)) {
                            corner[3]++;
                        }
                    }

                }
            }

            for(int i=0; i<4; ++i){
                if(corner[i] == 3)
                return true; // Cannot toggle if it mean creating a full square SERH
            }
            return false;
        }

        std::optional<Secteur> toggleRune(Path path, int depth = 0) {
            if(isLeaf()) {
                return cell.toggle();
            }else{
                if(path.empty()) {
                    throw std::invalid_argument("Path cannot be empty when toggling a rune.");
                }

                if(!isLocked(path.at(depth))){
                    auto sub = getSubGrid(path.at(depth)).toggleRune(path, depth+1);
                    if(sub == Secteur::FAILLE) {
                        updateSecteur();
                    }else if(sub == Secteur::SERH) {
                        cell.setState(Secteur::SERH);
                    }
                    return getSecteur();
                }
            }
            return std::nullopt;
        }
};

}