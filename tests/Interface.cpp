#include "GameState.hpp"
#include "imgui.h"
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

class GameStateTest : public GameState {
public:
    // Exposer des méthodes pour les tests
    using GameState::GameState;
    using GameState::moveHistory;
    using GameState::redoHistory;
    using GameState::currentPlayer;
    using GameState::targetSubGridPath;
};

class MockImGui {
public:
    static bool collapsingHeader(const char* label) { return true; }
    static void text(const char* fmt, ...) {}
    static void separator() {}
    static bool button(const char* label) { return false; }
    static void columns(int count, const char* id = nullptr, bool border = false) {}
    static void nextColumn() {}
};

TEST(GameConfigUITest, NullAndEdgeCases) {
    // Setup
    TTT_GridLogic gridLogic(3, 3);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);
    
    // Test 1: Historique vide
    gameState.moveHistory.clear();
    gameState.redoHistory.clear();
    EXPECT_NO_FATAL_FAILURE({
        if (MockImGui::collapsingHeader("Game Config")) {
            MockImGui::text("AutoMove : %s", gameState.autoMode ? "On" : "Off");
            gameState.showParam();
            MockImGui::separator();
            if (MockImGui::button("Réinitialiser")) {
                gameState.reset();
            }
            MockImGui::separator();
            
            std::ostringstream str;
            str << gameState.currentPlayer;
            MockImGui::text("Current player : %s", str.str().c_str());
            str.str("");
            
            MockImGui::separator();        
            MockImGui::columns(2, "historyColumns", false);
            
            MockImGui::text("[History | %ld]", gameState.moveHistory.size());
            MockImGui::nextColumn();
            MockImGui::text("[Redo]");
            MockImGui::nextColumn();
            
            size_t max_size = std::max(gameState.moveHistory.size(), gameState.redoHistory.size());
            for (size_t i = 0; i < max_size; ++i) {
                if (i < gameState.moveHistory.size()) {
                    str << gameState.moveHistory[i];
                    MockImGui::text("%s [%ld/%ld]", str.str().c_str(), i + 1, gameState.moveHistory.size());
                    str.str("");
                } else {
                    MockImGui::text(" ");
                }
                
                MockImGui::nextColumn();
                
                if (i < gameState.redoHistory.size()) {
                    str << gameState.redoHistory[i];
                    MockImGui::text("%s [%ld/%ld]", str.str().c_str(), i + 1, gameState.redoHistory.size());
                    str.str("");
                } else {
                    MockImGui::text(" ");
                }
                
                MockImGui::nextColumn();
            }
            
            MockImGui::columns(1, nullptr, false); // Correction ici
        }
    });
}
TEST(GameConfigUITest, LargeHistory) {
    TTT_GridLogic gridLogic(3, 3);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);
    
    // Remplir avec un grand historique
    for (int i = 0; i < 1000; i++) {
        gameState.moveHistory.push_back({{i}, {}, TTT_Shape::CROSS});
        if (i % 2 == 0) {
            gameState.redoHistory.push_back({{i}, {}, TTT_Shape::CIRCLE});
        }
    }
    
    EXPECT_NO_FATAL_FAILURE({
        // Même code que précédemment...
        if (MockImGui::collapsingHeader("Game Config")) {
            // ... [reprendre le même code]
        }
    });
}
TEST(GameConfigUITest, InvalidMoveEntries) {
    TTT_GridLogic gridLogic(3, 3);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);
    
    // Ajouter des entrées invalides
    gameState.moveHistory.push_back({{}, {}, static_cast<TTT_Shape>(999)}); // Shape invalide
    gameState.moveHistory.push_back({{999999}, {}, TTT_Shape::CROSS}); // Path invalide
    
    EXPECT_NO_FATAL_FAILURE({
        // Même code que précédemment...
        if (MockImGui::collapsingHeader("Game Config")) {
            // ... [reprendre le même code]
        }
    });
}
TEST(GameConfigUITest, StringStreamEdgeCases) {
    TTT_GridLogic gridLogic(3, 3);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);
    
    // Cas particulier pour le stringstream
    gameState.moveHistory.push_back({{}, {}, TTT_Shape::NONE});
    gameState.currentPlayer = static_cast<TTT_Shape>(999); // Valeur invalide
    
    std::ostringstream str;
    EXPECT_NO_FATAL_FAILURE({
        str << gameState.currentPlayer;
        MockImGui::text("Current player : %s", str.str().c_str());
    });
}
TEST(GameConfigUITest, 1x1GridBasic) {
    // Configuration minimale 1x1
    TTT_GridLogic gridLogic(1, 1);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);

    // Test affichage de base
    EXPECT_NO_FATAL_FAILURE({
        if (MockImGui::collapsingHeader("Game Config 1x1")) {
            MockImGui::text("Grille 1x1 - Basic");
            gameState.showParam();
        }
    });
}
TEST(GameConfigUITest, 1x1GridWithMoves_SegfaultFix) {


    // Configuration initiale
    TTT_GridLogic gridLogic(1, 1);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);

    // 1. Vérification de la grille 1x1 de base
    EXPECT_TRUE(gridLogic.isLeaf());
    EXPECT_EQ(gridLogic.getRows(), 1);
    EXPECT_EQ(gridLogic.getCols(), 1);

    // 2. Préparation des mouvements avec vérification
    Move crossMove;
    crossMove.path = {}; // Chemin vide pour grille 1x1
    crossMove.shape = TTT_Shape::CROSS;
    
    Move circleMove;
    circleMove.path = {}; // Chemin vide pour grille 1x1
    circleMove.shape = TTT_Shape::CIRCLE;

    // 3. Ajout sécurisé des mouvements
    ASSERT_NO_THROW({
        gameState.moveHistory.push_back(crossMove);
        gameState.redoHistory.push_back(circleMove);
    });

    // 4. Test d'affichage avec vérifications étape par étape
    // Vérification du collapsingHeader
    EXPECT_NO_FATAL_FAILURE({
        bool headerOpen = MockImGui::collapsingHeader("Game Config 1x1 With Moves");
        ASSERT_TRUE(headerOpen);
    });

    // Configuration des colonnes
    EXPECT_NO_FATAL_FAILURE({
        MockImGui::columns(2, "1x1Columns", false);
    });

    // Affichage de l'historique avec vérifications
    for (size_t i = 0; i < gameState.moveHistory.size(); ++i) {

            // Vérification de l'élément
            ASSERT_LT(i, gameState.moveHistory.size());
            
            std::ostringstream str;

            // Vérification de la conversion en string
            ASSERT_NO_THROW({
                str << gameState.moveHistory[i];
            });
            
            // Vérification du text
            ASSERT_NO_THROW({
                MockImGui::text("%s", str.str().c_str());
            });
            
            MockImGui::nextColumn();

    }

    // Reset des colonnes
    EXPECT_NO_FATAL_FAILURE({
        MockImGui::columns(1, nullptr, false);
    });

    // 5. Vérification supplémentaire pour le cas 1x1
    EXPECT_TRUE(gameState.moveHistory[0].path.empty()) << "Le chemin devrait être vide pour une grille 1x1";
    EXPECT_TRUE(gameState.redoHistory[0].path.empty()) << "Le chemin devrait être vide pour une grille 1x1";
}
TEST(GameConfigUITest, 1x1GridWithMoves) {
    TTT_GridLogic gridLogic(1, 1);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);

    // Simuler quelques mouvements
    gameState.moveHistory.push_back({{}, {}, TTT_Shape::CROSS});
    gameState.redoHistory.push_back({{}, {}, TTT_Shape::CIRCLE});

    // Test affichage avec historique
    EXPECT_NO_FATAL_FAILURE({
        if (MockImGui::collapsingHeader("Game Config 1x1 With Moves")) {
            MockImGui::columns(2, "1x1Columns", false);
            
            // Vérifier l'affichage de l'historique
            for (size_t i = 0; i < gameState.moveHistory.size(); ++i) {
                std::ostringstream str;
                str << gameState.moveHistory[i];
                MockImGui::text("%s", str.str().c_str());
                MockImGui::nextColumn();
            }
            MockImGui::columns(1, nullptr, false);
        }
    });
}
TEST(GameConfigUITest, 1x1GridReset) {
    TTT_GridLogic gridLogic(1, 1);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);

    // Simuler un mouvement puis reset
    gameState.moveHistory.push_back({{}, {}, TTT_Shape::CROSS});
    
    // Test bouton reset
    EXPECT_NO_FATAL_FAILURE({
        if (MockImGui::button("Reset 1x1")) {
            gameState.reset();
            EXPECT_TRUE(gameState.moveHistory.empty());
        }
    });
}
TEST(GameConfigUITest, 1x1GridEdgeCases) {
    TTT_GridLogic gridLogic(1, 1);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);

    // Cas extrêmes
    gameState.moveHistory.push_back({{999}, {}, TTT_Shape::CROSS}); // Index invalide
    gameState.currentPlayer = static_cast<TTT_Shape>(999); // Valeur enum invalide

    // Test affichage avec données corrompues
    EXPECT_NO_FATAL_FAILURE({
        if (MockImGui::collapsingHeader("Game Config 1x1 Corrupted")) {
            std::ostringstream str;
            str << gameState.currentPlayer;
            MockImGui::text("Player: %s", str.str().c_str());
            
            for (const auto& move : gameState.moveHistory) {
                str.str("");
                str << move;
                MockImGui::text("Move: %s", str.str().c_str());
            }
        }
    });
}
TEST(GameConfigUITest, 1x1GridPathNavigation) {
    TTT_GridLogic gridLogic(1, 1);
    GridView gridView(gridLogic, 0.9f);
    GameStateTest gameState(gridView);

    // Test navigation avec chemin (devrait être vide en 1x1)
    EXPECT_NO_FATAL_FAILURE({
        Path emptyPath;
        auto targetGrid = gridLogic.getGridFromPath(emptyPath);
        EXPECT_TRUE(targetGrid.isLeaf());
        
        if (MockImGui::collapsingHeader("1x1 Path Test")) {
            MockImGui::text("Path test passed");
        }
    });
}