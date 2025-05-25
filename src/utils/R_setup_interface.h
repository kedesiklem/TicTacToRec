#pragma once

#include "../Grid/Rune/R_State.hpp"

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

namespace Rune {

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

    void show_grid_window(RuneState& state) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoScrollbar);
        
        ImVec2 window_pos = ImGui::GetCursorScreenPos();
        ImVec2 window_size = ImGui::GetContentRegionAvail();
    
        if (window_size.x > 0 && window_size.y > 0) {  // Safety check
            float min_size = std::min(window_size.x, window_size.y);
            const ImVec2 marging = {30,30};
            ImVec2 available_size = ImVec2{min_size, min_size} - (marging * 2);
    
            if (available_size.x > 0 && available_size.y > 0) {
                state.view.update(window_pos + marging, available_size);
    
                state.draw();
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
    void setup_interface(RuneState& state) {
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
        show_grid_window(state);
    };

} // namespace