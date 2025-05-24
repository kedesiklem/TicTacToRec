#include "TTT/TTT_GridLogic.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

using namespace std;

using namespace TTT;

// Helper function to create a path
Path make_path(const vector<int>& indices) {
    return Path(indices.begin(), indices.end());
}

// Tests for basic initialization
TEST(TTT_GridLogicTest, Initialization) {
    // Test minimal grid (1x1)
    TTT_GridLogic minimalGrid(1, 1);
    EXPECT_TRUE(minimalGrid.isLeaf());
    EXPECT_EQ(minimalGrid.getRows(), 1);
    EXPECT_EQ(minimalGrid.getCols(), 1);
    EXPECT_EQ(minimalGrid.getShape(), TTT_Shape::NONE);

    // Test 3x3 grid
    TTT_GridLogic grid3x3(3, 3);
    EXPECT_FALSE(grid3x3.isLeaf());
    EXPECT_EQ(grid3x3.getRows(), 3);
    EXPECT_EQ(grid3x3.getCols(), 3);
}

// Tests for subgrid access
TEST(TTT_GridLogicTest, SubGridAccess) {
    TTT_GridLogic grid(2, 2);

    // Valid access
    EXPECT_NO_THROW(grid.getSubGrid(0, 0));
    EXPECT_NO_THROW(grid.getSubGrid(1));

    // Invalid access (should throw)
    EXPECT_THROW(grid.getSubGrid(2, 0), out_of_range);
    EXPECT_THROW(grid.getSubGrid(-1), out_of_range);
}

// Tests for victory conditions
TEST(TTT_GridLogicTest, VictoryConditions) {
    // Test row victory
    TTT_GridLogic grid(3, 3);
    grid.getSubGrid(0, 0).setShape(TTT_Shape::CROSS);
    grid.getSubGrid(0, 1).setShape(TTT_Shape::CROSS);
    grid.getSubGrid(0, 2).setShape(TTT_Shape::CROSS);
    EXPECT_EQ(grid.checkVictory(), TTT_Shape::CROSS);

    // Test column victory
    grid.resetGrid();
    grid.getSubGrid(0, 1).setShape(TTT_Shape::CIRCLE);
    grid.getSubGrid(1, 1).setShape(TTT_Shape::CIRCLE);
    grid.getSubGrid(2, 1).setShape(TTT_Shape::CIRCLE);
    EXPECT_EQ(grid.checkVictory(), TTT_Shape::CIRCLE);

    // Test diagonal victory
    grid.resetGrid();
    grid.getSubGrid(0, 0).setShape(TTT_Shape::CROSS);
    grid.getSubGrid(1, 1).setShape(TTT_Shape::CROSS);
    grid.getSubGrid(2, 2).setShape(TTT_Shape::CROSS);
    EXPECT_EQ(grid.checkVictory(), TTT_Shape::CROSS);

    // Test draw condition
    grid.resetGrid();
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if(c<2 && r<2){
                grid.getSubGrid(r, c).setShape((r + c) % 2 == 0 ? TTT_Shape::CROSS : TTT_Shape::CIRCLE);
            }else{
                grid.getSubGrid(r, c).setShape((r + c) % 2 == 1 ? TTT_Shape::CROSS : TTT_Shape::CIRCLE);
            }
        }
    }
    EXPECT_EQ(grid.checkVictory(), TTT_Shape::DRAW);
}

// Tests for move playing and undoing
TEST(TTT_GridLogicTest, PlayAndUndoMoves) {
    TTT_GridLogic grid(3, 3, 1); // Depth 1 grid
    Path path = make_path({1, 2}); // Path to a subgrid

    // Test valid move
    EXPECT_TRUE(grid.playMove(path, TTT_Shape::CROSS));
    EXPECT_EQ(grid.getGridFromPath(path).getShape(), TTT_Shape::CROSS);

    // Test undo
    grid.undoMove(path);
    EXPECT_EQ(grid.getGridFromPath(path).getShape(), TTT_Shape::NONE);

    // Test invalid move (locked subgrid)
    grid.getSubGrid(1).setShape(TTT_Shape::CIRCLE);
    EXPECT_FALSE(grid.playMove(path, TTT_Shape::CROSS));
}

// Tests for path navigation
TEST(TTT_GridLogicTest, PathNavigation) {
    TTT_GridLogic grid(2, 2, 1); // Depth 2 grid (2x2x2)
    
    // Test valid path
    Path valid_path = make_path({0, 1});
    EXPECT_NO_THROW(grid.getGridFromPath(valid_path));
    
    // Test invalid path
    Path invalid_path = make_path({5, 10});
    EXPECT_THROW(grid.getGridFromPath(invalid_path), out_of_range);
    
    // Test path to leaf
    auto leaf = grid.getGridFromPath(valid_path);
    EXPECT_TRUE(leaf.isLeaf());
}

// Tests for available moves
TEST(TTT_GridLogicTest, AvailableMoves) {
    TTT_GridLogic grid(2, 2, 1); // Depth 2 grid
    Path target = make_path({0}); // Target first subgrid

    // Get available moves
    auto moves = grid.getAvailableMoves(target);
    EXPECT_EQ(moves.size(), 4); // 4 leaves in targeted subgrid

    // Test random move
    auto random_move = grid.getRandomAvailableMove(target);
    EXPECT_TRUE(random_move.has_value());
    EXPECT_EQ(random_move->size(), 2); // Path to a leaf (depth 2)
}

// Tests for edge cases
TEST(TTT_GridLogicTest, EdgeCases) {
    // Invalid grid dimensions
    EXPECT_THROW(TTT_GridLogic(0, 0), invalid_argument);
    
    // Negative depth
    EXPECT_THROW(TTT_GridLogic(2, 2, -1), invalid_argument);
    
    // Test locked state
    TTT_GridLogic grid(2, 2);
    grid.setShape(TTT_Shape::CROSS);
    EXPECT_TRUE(grid.isLocked());
    EXPECT_FALSE(grid.playMove(make_path({0}), TTT_Shape::CIRCLE));
}

// Test reset functionality
TEST(TTT_GridLogicTest, ResetGrid) {
    TTT_GridLogic grid(2, 2);
    grid.getSubGrid(0, 0).setShape(TTT_Shape::CROSS);
    grid.resetGrid();
    EXPECT_EQ(grid.getSubGrid(0, 0).getShape(), TTT_Shape::NONE);
    EXPECT_FALSE(grid.isLocked());
}

// Test shape conversion
TEST(TTT_GridLogicTest, ShapeConversion) {
    EXPECT_EQ(TTT_Cell::StringToShape("X"), TTT_Shape::CROSS);
    EXPECT_EQ(TTT_Cell::StringToShape("O"), TTT_Shape::CIRCLE);
    EXPECT_EQ(TTT_Cell::StringToShape("N"), TTT_Shape::NONE);
    EXPECT_EQ(TTT_Cell::StringToShape("="), TTT_Shape::DRAW);
    
    EXPECT_EQ(TTT_Cell::ShapeToString(TTT_Shape::CROSS), "X");
    EXPECT_EQ(TTT_Cell::ShapeToString(TTT_Shape::CIRCLE), "O");
    EXPECT_EQ(TTT_Cell::ShapeToString(TTT_Shape::NONE), "N");
    EXPECT_EQ(TTT_Cell::ShapeToString(TTT_Shape::DRAW), "=");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}