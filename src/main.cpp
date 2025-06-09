#define IMGUI_DEFINE_MATH_OPERATORS
#define DEBUG

#include <iostream>
#include "UI/TTT_setup_interface.h"
#include "UI/TTT_setup_shortcut.h"
#include "UI/config.h"


using namespace TTT;

#define GRID_SIZE 3
#define GRID_REC 1

int main() {

    srand(time(nullptr));

    // Initialisation GLFW
    if (!glfwInit()) {
        std::cerr << "Échec de l'initialisation GLFW" << std::endl;
        return -1;
    }

    // Configuration fenêtre
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Création fenêtre
    GLFWwindow* window = glfwCreateWindow(getWidthScreen(), getHeightScreen(), "Jeu", nullptr, nullptr);
    if (!window) {
        std::cerr << "Échec création fenêtre" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialisation ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialisation jeu
    TTT_GridLogic grid(GRID_SIZE, GRID_SIZE, GRID_REC);
    TTT_GridView gridV(0.985);
    GameModeManager gameMode(grid, gridV);
    gameMode.changeGameMode("RandomStart");
    std::stringstream str;

    LoadFonts(io, 40);

    ImGui::FileBrowser loadFileDialog;
    ImGui::FileBrowser saveFileDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter);
    
    loadFileDialog.SetTitle("Load File");
    loadFileDialog.SetTypeFilters({ ".ttt" });

    saveFileDialog.SetTitle("Save File");
    saveFileDialog.SetTypeFilters({ ".ttt" });

    ShortcutManager shortcutManager;
    setupShortcuts(shortcutManager, window, gameMode);

    shortcutManager.addShortcut({{ImGuiKey_O, true}}, 
        new Lambda([&](){loadFileDialog.Open();}), "loadFile");
    shortcutManager.addShortcut({{ImGuiKey_S, true}}, 
        new Lambda([&](){saveFileDialog.Open();}), "saveFile");

    bool actif = true;
    // Boucle principale
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Nouvelle frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        actif = !(saveFileDialog.IsOpened() || loadFileDialog.IsOpened());
        
        setup_interface(gameMode, actif);
        shortcutManager.update();

        saveFileDialog.Display();
        if(saveFileDialog.HasSelected())
        {
            gameMode.saveState(saveFileDialog.GetSelected().string());
            saveFileDialog.ClearSelected();
        }

        loadFileDialog.Display();
        if(loadFileDialog.HasSelected())
        {
            gameMode.loadState(loadFileDialog.GetSelected().string());
            loadFileDialog.ClearSelected();
        }

        // Rendu
        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Nettoyage
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}