#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

namespace {

    void setup_init(ImGuiID dockspace_id){
        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear any previous layout
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode);
        
        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);
        
        // Définir les zones de docking
        ImGui::DockBuilderDockWindow("Game", dock_main);
        ImGui::DockBuilderDockWindow("Options", dock_right);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    void setup_interface(){
            // Configuration de l'espace de docking
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
            
            // Création de la fenêtre principale de docking
            ImGui::Begin("DockSpace", nullptr, window_flags);
            ImGui::PopStyleVar(3);
            
            // Création de l'espace de docking
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            
            // Configuration initiale du docking (une seule fois)
            static bool first_time = true;
            if (first_time) {
                first_time = false;
                
                setup_init(dockspace_id);
            }
        }
}// namespace