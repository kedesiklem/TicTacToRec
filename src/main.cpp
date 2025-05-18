#define IMGUI_DEFINE_MATH_OPERATORS


#include <config.h>
#include <iostream>
#include <setup_interface.h>
#include <setup_shortcut.h>

#define GRID_SIZE 3
#define GRID_REC 1

int main() {
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
    GridLogic grid(GRID_SIZE, GRID_SIZE, GRID_REC);
    GridView gridV(grid, 0.985);
    MonteCarloBot bot;
    GameState gameState(gridV, bot);
    std::stringstream str;

    LoadFonts(io, 40);

    ShortcutManager shortcutManager;
    setupShortcuts(shortcutManager, window, gameState);

    // Boucle principale
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Nouvelle frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Configuration de l'interface
        setup_interface();
        
        // Affichage des fenêtres
        show_game_window(gameState);
        show_options_window(gameState);
        
        // Fin de la fenêtre de docking principale
        ImGui::End();

        shortcutManager.update();

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