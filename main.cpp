#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>

#include "TTT/GameState.h"

#define GRID_SIZE 3
#define GRID_REC 2

int main()
{
    // 1. Initialisation de GLFW - Gestion de la fenêtre et des entrées
    if (!glfwInit())
    {
        std::cerr << "Échec de l'initialisation de GLFW" << std::endl;
        return -1;
    }
    // Configuration des hints pour la création de la fenêtre OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // Version majeure d'OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // Version mineure d'OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Profil core (pas de fonctions dépréciées)
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Nécessaire sur MacOS
    #endif

    // 2. Création de la fenêtre
    GLFWwindow* window = glfwCreateWindow(
        720, 720,                              // Largeur, Hauteur
        "Fenêtre Minimaliste GLFW + ImGui",     // Titre
        nullptr,                                // Mode plein écran (none = windowed)
        nullptr                                 // Partage de contexte (none = non)
    );
    if (!window)
    {
        std::cerr << "Échec de la création de la fenêtre GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Définit la fenêtre comme le contexte OpenGL courant
    glfwMakeContextCurrent(window);
    // Active la synchronisation verticale (1 = activé, 0 = désactivé)
    glfwSwapInterval(1);

    // 3. Initialisation de ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Configuration de ImGui (paramètres par défaut explicites)
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Activation des contrôles clavier
    // Style par défaut
    ImGui::StyleColorsDark();
    // Initialisation des backends ImGui pour GLFW et OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");  // Spécifie la version GLSL

    // --- Composant ImGui - Start--------------------


    Grid mainGrid(GRID_SIZE, GRID_SIZE, GRID_REC, 150.f, 7.f);
    GameState gameState = GameState();
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None; // Supprimez NoMove et NoResize



    // --- Composant ImGui - Start--------------------

    // 4. Boucle principale
    while (!glfwWindowShouldClose(window))
    {
        // Gestion des événements (entrées, fenêtre, etc.)
        glfwPollEvents();
        // Début d'une nouvelle frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- Fenetre ImGui - Start--------------------

        if(ImGui::IsKeyPressed(ImGuiKey_R)) {
            mainGrid.resetGrid();
            gameState.reset();
        }

        // Obtenez la taille de la fenêtre principale
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        
        // Créez une fenêtre transparente sans décoration pour couvrir toute la fenêtre principale
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("MainWindow", nullptr, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoBackground);


        ImVec2 window_pos = ImGui::GetCursorScreenPos(); // Position absolue de la fenêtre
        ImVec2 window_size = ImGui::GetContentRegionAvail();
        

        // Bloque le déplacement seulement si la souris est sur la grille
        ImGuiWindow* current_window = ImGui::GetCurrentWindow();
        ImRect grid_rect(
            ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x + mainGrid.getTotalWidth(),
                   ImGui::GetCursorScreenPos().y + mainGrid.getTotalHeight())
        );
        if (grid_rect.Contains(ImGui::GetMousePos())) {
            current_window->Flags |= ImGuiWindowFlags_NoMove;
        } else {
            current_window->Flags &= ~ImGuiWindowFlags_NoMove;
        }


        mainGrid.setWindowSize(window_size.x, window_size.y);
        
        // mainGrid.update(gameState, gameState.targetSubGridPath, window_pos);
        gameState.updatev2(window_pos, mainGrid);

        mainGrid.getGridFromPath(gameState.targetSubGridPath).setShape(gameState.currentPlayer);

        mainGrid.drawv2(window_pos, gameState.targetSubGridPath);


        ImGui::End();
        ImGui::PopStyleVar();

        
        // --- Fenetre ImGui - End----------------------

        // Rendu
        ImGui::Render();
        // // Efface l'écran avec une couleur de fond
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Dessine les données ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Échange les buffers (double buffering)
        glfwSwapBuffers(window);
    }

    // 5. Nettoyage
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}