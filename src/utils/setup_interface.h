#pragma once

#include "GameState.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#define SAVE_WIN_FILE "win.ttt"

namespace {

    void setup_init(ImGuiID dockspace_id) {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar);
        
        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);
        
        ImGui::DockBuilderDockWindow("Game", dock_main);
        ImGui::DockBuilderDockWindow("Options", dock_right);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    void setup_interface() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);
        
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        
        static bool first_time = true;
        if (first_time) {
            first_time = false;
            setup_init(dockspace_id);
        }
    };

    void show_game_window(GridLogic& mainGrid, GameState& gameState) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoScrollbar);
        
        ImVec2 window_pos = ImGui::GetCursorScreenPos();
        ImVec2 window_size = ImGui::GetContentRegionAvail();

        float min_size = std::min(window_size.x, window_size.y);
        const ImVec2 marging = {30,30};
        
        // Création de la vue avec le ratio contrôlé par le slider
        GridView gridView(mainGrid, .98f);

        gridView.update(window_pos + marging, ImVec2{min_size, min_size} - (marging * 2));

        gameState.playTurn(gridView);

        gridView.draw(gameState.targetSubGridPath);
        
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void show_options_window(GameState& gameState) {
        ImGui::Begin("Options", nullptr);
        {
            ImGui::Text("Paramètres du jeu");
            ImGui::Separator();
            if (ImGui::Button("Réinitialiser")) {
                gameState.reset();
            }
            
            ImGui::Separator();

            std::ostringstream str;
            str << gameState.currentPlayer;
            ImGui::Text("Current player : %s", str.str().c_str());
            str.str("");

            ImGui::Separator();        
            ImGui::Columns(2, "historyColumns", false);

            ImGui::Text("[History]");
            ImGui::NextColumn();
            ImGui::Text("[Redo]");
            ImGui::NextColumn();

            size_t max_size = std::max(gameState.moveHistory.size(), gameState.redoHistory.size());
            for (size_t i = 0; i < max_size; ++i) {
                if (i < gameState.moveHistory.size()) {
                    str << gameState.moveHistory[i];
                    ImGui::Text("%s", str.str().c_str());
                    str.str("");
                } else {
                    ImGui::Text(" ");
                }
                
                ImGui::NextColumn();

                if (i < gameState.redoHistory.size()) {
                    str << gameState.redoHistory[i];
                    ImGui::Text("%s", str.str().c_str());
                    str.str("");
                } else {
                    ImGui::Text(" ");
                }
                
                ImGui::NextColumn();
            }

            ImGui::Columns(1);
        }
        ImGui::End();
    };

} // namespace