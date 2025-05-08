#include <config.h>
#include <iostream>
#include <setup_interface.h>
#include <shortcut_editor.h>
#include <GameState.h>

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
    Grid mainGrid(GRID_SIZE, GRID_SIZE, GRID_REC, 150.f, 20.f);
    GameState gameState;
    std::stringstream str;

    LoadFonts(io, 40);

    // Boucle principale
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Nouvelle frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Gestion inputs
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) glfwSetWindowShouldClose(window, true);
        if (ImGui::IsKeyPressed(ImGuiKey_R)) {
            mainGrid.resetGrid();
            gameState.reset();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_U)) {
            gameState.undoLastMove(mainGrid);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_I)) {
            gameState.redoLastMove(mainGrid);
        }

        setup_interface();

        // Fenêtre du jeu (maintenant dockée)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        
        ImGui::Begin("Game", nullptr 
            ,ImGuiWindowFlags_None
        );
        
        // Mise à jour jeu
        ImVec2 window_pos = ImGui::GetCursorScreenPos();
        ImVec2 window_size = ImGui::GetContentRegionAvail();
        
        mainGrid.setWindowSize(window_size.x, window_size.y);
        gameState.update(window_pos, mainGrid);
        mainGrid.getGridFromPath(gameState.targetSubGridPath).setShape(gameState.currentPlayer);
        mainGrid.draw(window_pos, gameState.targetSubGridPath);

        ImGui::End();
        ImGui::PopStyleVar();

        // Fenêtre des options (dockée à droite)
        ImGui::Begin("Options", nullptr);
        {
            ImGui::Text("Paramètres du jeu");
            ImGui::Separator();
            if (ImGui::Button("Réinitialiser")) {
                mainGrid.resetGrid();
                gameState.reset();
            }
        
            ImGui::Separator();

            str << gameState.currentPlayer;
            ImGui::Text("Current player : %s", str.str().c_str());
            str.str("");


            ImGui::Separator();
            size_t max_size = std::max(gameState.moveHistory.size(), gameState.redoHistory.size());
           
            // Oui je sais, c'est à l'arrache mais ça fait le job
            for (size_t i = 0; i < max_size; ++i) {
                if (i < gameState.moveHistory.size())
                    str << gameState.moveHistory[i];
                else str << "\t\t";
                str << "\t\t";
                if (i < gameState.redoHistory.size())
                    str << gameState.redoHistory[i];
                str << "\n";
            }
            ImGui::Text("[History] [Redo] \n%s", str.str().c_str());
            str.str("");
        }
        ImGui::End();

        // Fin de la fenêtre de docking principale
        ImGui::End();

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