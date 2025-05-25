#pragma once

#include "../Grid/Rune/R_State.hpp"

#include "emacsStyleShortCut.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>

#define SAVE_STATE "save_state.ttt"

namespace Rune {

class CloseWindowFunctor : public Functor {
    GLFWwindow* window;
public:
    CloseWindowFunctor(GLFWwindow* w) : window(w) {}
    void exec() override { glfwSetWindowShouldClose(window, true); }
};

void setupShortcuts(ShortcutManager& shortcutManager, GLFWwindow* window) {
    
    shortcutManager.addShortcut({{ImGuiKey_Escape}}, 
        new CloseWindowFunctor(window), "Fermer la fenêtre");

}

}