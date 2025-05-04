#include "GameState.h"


// wrapper pour update
bool GameState::updatev2(const ImVec2& window_pos, Grid& grid){
    std::vector<int> currentPath = {};
    std::vector<int> finalPath = {};
    return updatev2(window_pos, grid, currentPath, finalPath, 0);
}

bool GameState::updatev2(const ImVec2& window_pos, Grid& grid, std::vector<int> currentPath, std::vector<int>& finalPath, int recursionLevel){
    ImVec2 mouse_pos = ImGui::GetMousePos();
    bool mouse_left_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    // Si locked, pas d'interaction
    if(grid.isLockedShaped()){
        grid.setHovered(false);
        return false;
    }

    size_t cellSize = grid.getCellSize();
    size_t padding = grid.getPadding();

    if(grid.isLeaf()){
        // Si on atteint une feuille : on est sur une cible
        grid.setHovered(false);
        if(mouse_pos.x >= window_pos.x && mouse_pos.x < window_pos.x + cellSize &&
           mouse_pos.y >= window_pos.y && mouse_pos.y < window_pos.y + cellSize){
            grid.setHovered(true);

            if(mouse_left_clicked && grid.getShape() == GridShape::NONE){
                grid.setShape(currentPlayer);
                finalPath = currentPath; // Sauvegarde le chemin complet
                return true;
            }
        }
        return false;
    }
    // Sinon on est pas sur une feuille : on iter sur toutes les sous-grilles en n'appelant que les cibles (ou toutes si pas de cible)
    bool updated = false;
    for(int r = 0; r < grid.getRows(); ++r){
        for(int c = 0; c < grid.getCols(); ++c){

            // Verifier si on est sur la case cible
            if(!targetSubGridPath.empty() && recursionLevel < targetSubGridPath.size()){
                int index = targetSubGridPath[recursionLevel];
                int target_r = index / grid.getCols();
                int target_c = index % grid.getCols();
                if(r != target_r || c != target_c){
                    continue; // On ne traite pas cette case
                }
            }

            float x = window_pos.x + c * (cellSize + padding);
            float y = window_pos.y + r * (cellSize + padding);

            // On ajoute la case courante au chemin
            currentPath.push_back(r * grid.getCols() + c);
            if(updatev2(ImVec2(x, y), grid.getSubGrid(r,c), currentPath, finalPath, recursionLevel + 1)){
                grid.setShape(grid.checkVictory());
                updated = true;
            }
            currentPath.pop_back();
        }
    }

    if(recursionLevel == 0 && updated){
        endTurn(finalPath, grid); // Utilise le chemin complet sauvegardé
        std::cout << *this << std::endl;
    }

    return updated;
}

void GameState::endTurn(const std::vector<int>& lastPlayedSubGridPath, Grid& grid) {
  if (lastPlayedSubGridPath.empty()) {
    targetSubGridPath.clear();
    currentPlayer = (currentPlayer == GridShape::CROSS) ? GridShape::CIRCLE : GridShape::CROSS;
    return;
  }

  targetSubGridPath = lastPlayedSubGridPath;
  targetSubGridPath.erase(targetSubGridPath.begin());

  while(grid.getGridFromPath(targetSubGridPath).getShape() != GridShape::NONE && !targetSubGridPath.empty()) {
    targetSubGridPath.erase(targetSubGridPath.begin());
  }
    
  currentPlayer = (currentPlayer == GridShape::CROSS) ? GridShape::CIRCLE : GridShape::CROSS;
}

std::ostream& operator<<(std::ostream& os, const GameState& gameState){
  os << "GameState: " << std::endl;
  os << "Current Player: " << gameState.currentPlayer << std::endl;
  os << "Target SubGrid Path: ";
  for (const auto& path : gameState.targetSubGridPath) {
    os << path << " ";
  }
  os << std::endl;
  return os;
}
