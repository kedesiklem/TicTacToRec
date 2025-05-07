#pragma once

#include <fstream>
#include <map>
#include <sstream>
#include <imgui.h>
#include <iostream>
#include <functional>
#include "Command.h"

namespace {
    class Shortcut {
        public:
            inline static const std::string DEFAULT_CATEGORIE = "General";

            ImGuiKey key = (ImGuiKey)0;
            bool ctrl = false;
            bool alt = false;
            bool shift = false;
            bool waitingForInput = false;
            std::string category = DEFAULT_CATEGORIE;
            Command func;

            std::string GetKeyName() const {
                std::string name;
                if (ctrl) name += "Ctrl + ";
                if (alt) name += "Alt + ";
                if (shift) name += "Shift + ";
                name += ImGui::GetKeyName(key);
                return name;
            }
    };

    const std::string CONFIG_FILE = "shortcuts.ini";

    bool IsModifierKey(ImGuiKey key) {
        return key == ImGuiKey_ModAlt || key == ImGuiKey_ModCtrl || key == ImGuiKey_ModShift || 
               key == ImGuiKey_LeftAlt || key == ImGuiKey_LeftCtrl || key == ImGuiKey_LeftShift ||
               key == ImGuiKey_RightAlt || key == ImGuiKey_RightCtrl || key == ImGuiKey_RightShift ||
               key == ImGuiKey_ReservedForModCtrl || key == ImGuiKey_ReservedForModShift || key == ImGuiKey_ReservedForModAlt;
    }

    // Sauvegarde des raccourcis dans un fichier .ini
    void SaveShortcuts(const std::map<std::string, Shortcut> &shortcuts) {
        std::ofstream file(CONFIG_FILE);
        if (!file.is_open()) {
            std::cerr << "Erreur: impossible d'ouvrir le fichier de configuration pour la sauvegarde." << std::endl;
            return;
        }

        for (const auto& [name, shortcut] : shortcuts) {
            file << name << "=" << (int)shortcut.key << " "
                 << shortcut.ctrl << " " << shortcut.alt << " " << shortcut.shift << "\n";
        }
        file.close();
    }

    // Chargement des raccourcis depuis un fichier .ini
    void LoadShortcuts(std::map<std::string, Shortcut> &shortcuts) {
        std::ifstream file(CONFIG_FILE);
        if (!file.is_open()) {
            std::cerr << "Erreur: impossible d'ouvrir le fichier de configuration pour le chargement." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name;
            int key;
            bool ctrl, alt, shift;

            if (std::getline(iss, name, '=') && iss >> key >> ctrl >> alt >> shift) {
                if (shortcuts.find(name) != shortcuts.end()) {
                    shortcuts[name].key = (ImGuiKey)key;
                    shortcuts[name].ctrl = ctrl;
                    shortcuts[name].alt = alt;
                    shortcuts[name].shift = shift;
                }
            }
        }
        file.close();
    }

    // Interface pour modifier les raccourcis (magouille infamme à refaire au propre)
    void ShowShortcutEditor(bool &showShortcutEditor, std::map<std::string, Shortcut> &shortcuts) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            showShortcutEditor = false;
        }
    
        ImGui::Begin("Éditeur de raccourcis", &showShortcutEditor, window_flags);
    
        static bool first = true;
        static std::map<std::string, std::vector<std::pair<std::string, std::reference_wrapper<Shortcut>>>> categorizedShortcuts;

        if(first){
            // Group shortcuts by category
            for (auto& [name, shortcut] : shortcuts) {
                categorizedShortcuts[shortcut.category].push_back(std::pair<std::string, std::reference_wrapper<Shortcut>>(name, shortcut));
            }
            first = false;
        }
        // Iterate through the categories and display the shortcuts
        for (auto& [category, categoryShortcuts] : categorizedShortcuts) {
            ImGui::Text("%s", category.c_str());
    
            for (auto& [name, shortcut] : categoryShortcuts) {
                ImGui::Text("%s :", name.c_str());
                ImGui::SameLine();
    
                if (shortcut.get().waitingForInput) {
                    ImGui::Text("[Appuyez sur une touche...]");
    
                    for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key) {
                        if (ImGui::IsKeyPressed((ImGuiKey)key) && !IsModifierKey((ImGuiKey)key)) {
                            shortcut.get().key = (ImGuiKey)key;
                            shortcut.get().ctrl = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
                            shortcut.get().alt = ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt);
                            shortcut.get().shift = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);
    
                            shortcut.get().waitingForInput = false;
                            SaveShortcuts(shortcuts);
                            break;
                        }
                    }
                } else {
                    if (ImGui::Button(shortcut.get().GetKeyName().c_str())) {
                        shortcut.get().waitingForInput = true;
                    }
                }
            }
            ImGui::Separator();
        }
    
        if (ImGui::Button("Charger config")) {
            LoadShortcuts(shortcuts);
        }
        ImGui::SameLine();
        if (ImGui::Button("Sauvegarder config")) {
            SaveShortcuts(shortcuts);
        }
    
        ImGui::End();
    }
    

    // Détection d'un raccourci clavier
    bool IsShortcutPressed(Shortcut& shortcut) {
        return (shortcut.key != 0) && (ImGui::IsKeyPressed(shortcut.key) && // Ensure shortcut.key is ImGuiKey
            (!(shortcut.ctrl)  || ImGui::IsKeyDown(ImGuiKey_ModCtrl)) &&
            (!(shortcut.alt)   || ImGui::IsKeyDown(ImGuiKey_ModAlt)) &&
            (!(shortcut.shift) || ImGui::IsKeyDown(ImGuiKey_ModShift))) &&
            (!(shortcut.waitingForInput)) ;
    }

    // TODO : evenemential
    void runShortcut(std::map<std::string, Shortcut> shortcuts){
        for (auto& [name, shortcut] : shortcuts) {
            if (IsShortcutPressed(shortcut)) {
                shortcut.func();
            }
        }
    }
} // namespace


/* ADD IN MAIN
std::map<std::string, PocHJ::Shortcut> shortcuts = {};

// ShortCut initialization
shortcuts["ToggleFullscreen"].func = [=](){fullscreen = !fullscreen;};
//...

// Recuperation des raccourcies existants;
PocHJ::LoadShortcuts(shortcuts);
*/