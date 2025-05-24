#pragma once

#include "../Grid/GameModeManager.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>

#define SAVE_WIN_FILE "win.ttt"

namespace {

    void setup_init(ImGuiID dockspace_id) {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar);
        
        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.3f, nullptr, &dock_main);
        
        ImGui::DockBuilderDockWindow("Game", dock_main);
        ImGui::DockBuilderDockWindow("Settings", dock_right);
        ImGui::DockBuilderDockWindow("Game Settings", dock_right);
        
        ImGui::DockBuilderFinish(dockspace_id);
    }

    void show_game_window(GameState& gameState) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoScrollbar);
        
        ImVec2 window_pos = ImGui::GetCursorScreenPos();
        ImVec2 window_size = ImGui::GetContentRegionAvail();
    
        if (window_size.x > 0 && window_size.y > 0) {  // Safety check
            float min_size = std::min(window_size.x, window_size.y);
            const ImVec2 marging = {30,30};
            ImVec2 available_size = ImVec2{min_size, min_size} - (marging * 2);
    
            if (available_size.x > 0 && available_size.y > 0) {
                gameState.grid.update(window_pos + marging, available_size);
    
                if(!gameState.grid.grid_root.isLocked()) {
                    gameState.playTurn();
                }
    
                gameState.grid.draw(gameState.targetSubGridPath);
            }
        }
        
        ImGui::End();
        ImGui::PopStyleVar();
    }
    void show_settings_window(ImGuiIO& io) {
        ImGui::Begin("Settings");
    
        // Font size control
        static float font_size = io.FontGlobalScale;
        ImGui::SliderFloat("Font Size", &font_size, 0.5f, 2.0f, "%.2f");
        if (ImGui::Button("Apply Font Size")) {
            io.FontGlobalScale = font_size;
        }
        ImGui::Separator();
        
        // Background color control
        static ImVec4 bg_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
        ImGui::ColorEdit4("Background Color", (float*)&bg_color);
        if (ImGui::Button("Apply Background Color")) {
            ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = bg_color;
        }
    
        ImGui::End();
    }
    void show_mode_window(GameModeManager& modeManager) {
        ImGui::Begin("Game Settings");
    
        // Première section déroulante pour les paramètres de la grille
        if (ImGui::CollapsingHeader("Grid Config")) {
            static int grid_cols = 3;
            static int grid_rows = 3;
            static int grid_depth = 1;
            static bool grid_squared = true;
    
            ImGui::Checkbox("Square Grid", &grid_squared);
    
            if (grid_squared) {
                ImGui::SliderInt("Grid Size", &grid_cols, 1, 5);
                grid_rows = grid_cols;
            } else {
                ImGui::SliderInt("Grid Columns", &grid_cols, 1, 5);
                ImGui::SliderInt("Grid Rows", &grid_rows, 1, 5);
            }
            ImGui::SliderInt("Grid Depth", &grid_depth, 0, 3);
    
            if (ImGui::Button("Apply Grid Settings")) {
                modeManager().grid.grid_root = TTT_GridLogic(grid_rows, grid_cols, grid_depth);
                modeManager().reset();
            }
        }
    
        // Deuxième section déroulante pour la sélection du mode de jeu
        if (ImGui::CollapsingHeader("Game Mode")) {
            const auto& modes = modeManager.getAvailableModes();
            ImGui::BeginGroup();
            // Afficher la liste des modes disponibles avec des boutons
            for (const auto& [name, _] : modes) {
                if (ImGui::Button(name.c_str())) {
                    modeManager.changeGameMode(name);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Switch to %s mode", name.c_str());
                }
                
                // Espacement entre les boutons
                ImGui::SameLine();
                if (ImGui::GetContentRegionAvail().x < ImGui::CalcTextSize(name.c_str()).x + 30) {
                    ImGui::NewLine();
                }
            }
            ImGui::EndGroup();
        }
        
        auto& gameState = modeManager();

        if (ImGui::CollapsingHeader("Game Config")) {
            ImGui::Text("AutoMove : %s", gameState.autoMode ? "On" : "Off");

            gameState.showParam();

            ImGui::Separator();
            if (ImGui::Button("Réinitialiser")) {
                gameState.reset();
            }
            
            ImGui::Separator();

            std::ostringstream str;
            str << gameState.currentPlayer;
            ImGui::Text("Current player : %s", str.str().c_str());
            str.str("");

            if (ImGui::CollapsingHeader("Move History")) {
                ImGui::Separator();        
                ImGui::Columns(2, "historyColumns", false);

                ImGui::Text("[History | %ld]", gameState.moveHistory.size());
                ImGui::NextColumn();
                ImGui::Text("[Redo]");
                ImGui::NextColumn();

                size_t max_size = std::max(gameState.moveHistory.size(), gameState.redoHistory.size());
                for (size_t i = 0; i < max_size; ++i) {
                    if (i < gameState.moveHistory.size()) {
                        str << gameState.moveHistory[i];
                        ImGui::Text("%s [%ld/%ld]", str.str().c_str(), i + 1, gameState.moveHistory.size());
                        str.str("");
                    } else {
                        ImGui::Text(" ");
                    }
                    
                    ImGui::NextColumn();

                    if (i < gameState.redoHistory.size()) {
                        str << gameState.redoHistory[i];
                        ImGui::Text("%s [%ld/%ld]", str.str().c_str(), i + 1, gameState.redoHistory.size());
                        str.str("");
                    } else {
                        ImGui::Text(" ");
                    }
                    
                    ImGui::NextColumn();
                }

                ImGui::Columns(1);

            }
        }
        
        ImGui::End();
    }
    void setup_interface(GameModeManager& modeManager) {
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
        show_settings_window(ImGui::GetIO());
        show_game_window(modeManager());
        show_mode_window(modeManager);
    };

} // namespace