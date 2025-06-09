#pragma once

#include "config.h"
#include "functor.hpp"
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <set>
#include <memory>

struct KeyCombo {
    ImGuiKey key;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;

    bool operator<(const KeyCombo& other) const {
        return std::tie(key, ctrl, alt, shift) < 
            std::tie(other.key, other.ctrl, other.alt, other.shift);
    };

    bool operator==(const KeyCombo& other) const {
        return std::tie(key, ctrl, alt, shift) == 
            std::tie(other.key, other.ctrl, other.alt, other.shift);
    };
};

class Shortcut {
    Functor* func;
    std::string description;

public:
    Shortcut(Functor* f, const std::string& desc = "")
        : func(f), description(desc) {};
    
    ~Shortcut() = default;
    
    void execute() { func->exec(); };
    
    std::string getDescription() const {
        return description;
    }
};

class ShortcutTreeNode {
public:
    std::map<KeyCombo, std::unique_ptr<ShortcutTreeNode>> children;
    std::unique_ptr<Shortcut> shortcut;

    ShortcutTreeNode() = default;
    ~ShortcutTreeNode() = default;

    void addSequence(const std::vector<KeyCombo>& sequence, size_t index, Functor* func, const std::string& desc) {
        if (index == sequence.size()) {
            shortcut = std::make_unique<Shortcut>(func, desc);
            return;
        }

        const auto& keyCombo = sequence[index];
        auto it = children.find(keyCombo);
        if (it == children.end()) {
            it = children.emplace(keyCombo, std::make_unique<ShortcutTreeNode>()).first;
        }
        it->second->addSequence(sequence, index + 1, func, desc);
    }

    ShortcutTreeNode* findNode(const std::vector<KeyCombo>& sequence, size_t index) {
        if (index == sequence.size()) {
            return this;
        }

        const auto& keyCombo = sequence[index];
        auto it = children.find(keyCombo);
        if (it == children.end()) {
            return nullptr;
        }
        return it->second->findNode(sequence, index + 1);
    }

    void collectDescriptions(std::vector<std::pair<std::vector<KeyCombo>, std::string>>& result, 
                            std::vector<KeyCombo>& currentSequence) const {
        if (shortcut) {
            result.emplace_back(currentSequence, shortcut->getDescription());
        }

        for (const auto& [keyCombo, child] : children) {
            currentSequence.push_back(keyCombo);
            child->collectDescriptions(result, currentSequence);
            currentSequence.pop_back();
        }
    }
};

class ShortcutManager {
    ShortcutTreeNode root;
    std::vector<KeyCombo> currentSequence;
    std::chrono::steady_clock::time_point lastKeyTime;
    static constexpr std::chrono::milliseconds sequenceTimeout{1000};

public:
    ~ShortcutManager() = default;

    void addShortcut(const std::vector<KeyCombo>& sequence, Functor* func, const std::string& desc = "") {
        root.addSequence(sequence, 0, func, desc);
    }

    void update() {
        const auto& io = ImGui::GetIO();
        auto now = std::chrono::steady_clock::now();

        // Reset sequence if timeout reached
        if (!currentSequence.empty() && 
            (now - lastKeyTime) > sequenceTimeout) {
            currentSequence.clear();
        }

        // Get the current node in the tree
        ShortcutTreeNode* currentNode = root.findNode(currentSequence, 0);
        if (!currentNode) {
            currentSequence.clear();
            return;
        }

        // Check all possible next keys from current node
        for (const auto& [keyCombo, child] : currentNode->children) {
            if (ImGui::IsKeyPressed(keyCombo.key) && 
                io.KeyCtrl == keyCombo.ctrl && 
                io.KeyAlt == keyCombo.alt && 
                io.KeyShift == keyCombo.shift) {
                
                currentSequence.push_back(keyCombo);
                lastKeyTime = now;

                // Check if this is a complete shortcut
                ShortcutTreeNode* nextNode = root.findNode(currentSequence, 0);
                if (nextNode && nextNode->shortcut) {
                    nextNode->shortcut->execute();
                    currentSequence.clear();
                    return;
                }

                break; // Only process one key per frame
            }
        }
    }

    std::string getSequenceString(const std::vector<KeyCombo>& sequence) const {
        std::string result;
        for (const auto& combo : sequence) {
            if (!result.empty()) result += " ";
            
            if (combo.ctrl) result += "C-";
            if (combo.alt) result += "M-";
            if (combo.shift) result += "S-";
            
            result += ImGui::GetKeyName(combo.key);
        }
        return result;
    }
    
    void drawHelp() const {
        if (ImGui::Begin("Shortcuts Help")) {
            ImGui::Text("Available Shortcuts:");
            ImGui::Separator();
            
            std::vector<std::pair<std::vector<KeyCombo>, std::string>> descriptions;
            std::vector<KeyCombo> currentSeq;
            root.collectDescriptions(descriptions, currentSeq);
            
            for (const auto& [sequence, desc] : descriptions) {
                ImGui::Text("%s: %s", 
                    getSequenceString(sequence).c_str(),
                    desc.empty() ? "No description" : desc.c_str());
            }
        }
        ImGui::End();
    }
};