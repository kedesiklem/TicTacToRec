#include "TTT_GameStateVariant.hpp"

using namespace TTT;


bool GameStateBot::isBotPlayer(TTT_Shape shape) {
    return (autoMode);
}

bool GameStateBot::playBot(){
    auto move = bot->getNextMove(*this);
    if(move){
        return playMove(move.value());
    }
    return false;
}

bool GameStateBot::playTurn() {
    if(isBotPlayer(currentPlayer)) {
        return playBot();
    } else {
        auto path = view.handleGridInteraction(grid);
        if(path) {
            return playMove(path.value());
        }
        return false;
    }
}

void GameStateBot::showParam(){
    ImGui::Text(("Current Bot : " + botName).c_str());
    if (ImGui::CollapsingHeader("Bot")) {
        ImGui::Separator();
        ImGui::Indent(10.0f);

        ImGui::BeginGroup();
        for (const auto& [name, _] : botFactories) {
            if (ImGui::Button((name + " Bot").c_str())) {
                changeBot(name);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Switch to %s bot", name.c_str());
            }
            
            // Espacement entre les boutons
            ImGui::SameLine();
            if (ImGui::GetContentRegionAvail().x < ImGui::CalcTextSize(name.c_str()).x + 30) {
                ImGui::NewLine();
            }
        }
        ImGui::EndGroup();

        ImGui::Unindent(10.0f);
        ImGui::Separator();
    }
}