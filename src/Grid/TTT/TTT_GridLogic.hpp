#pragma once

#include "../GridLogic.hpp"

namespace TTT {

enum class TTT_Shape {
    NONE,
    CROSS,
    CIRCLE,
    DRAW,
};

class TTT_Cell{
    TTT_Shape currentShape = TTT_Shape::NONE;
    public :

        operator TTT_Shape() const {
            return currentShape;
        }

        TTT_Cell operator= (TTT_Shape shape) {
            currentShape = shape;
            return currentShape;
        }

        TTT_Cell() = default;
        TTT_Cell(TTT_Shape shape) : currentShape(shape) {}

        static TTT_Shape nextShapePlayable(const TTT_Shape shape);
        static std::string ShapeToString(const TTT_Shape shape);
        static TTT_Shape StringToShape(const std::string& str);
        static bool isWinning(const TTT_Shape shape){
            return (shape == TTT_Shape::CROSS || shape == TTT_Shape::CIRCLE);
        }
        static bool isLocked(const TTT_Shape shape){
            return shape != TTT_Shape::NONE;
        }
        
        TTT_Shape nextShapePlayable() const {
            return nextShapePlayable(currentShape);
        }
        std::string ShapeToString() const {
            return ShapeToString(currentShape);
        }
        bool isWinning() const{
            return isWinning(currentShape);
        }
        bool isLocked() const{
            return isLocked(currentShape);
        }
};


class TTT_GridLogic : public GridLogic<TTT_Cell, TTT_GridLogic> {
    public:
        TTT_GridLogic(int rows, int cols, int depth = 0) : GridLogic<TTT_Cell, TTT_GridLogic>(rows, cols, depth) {}
        TTT_Shape checkVictory();

        const TTT_Shape getShape() const {
            return cell;
        };

        bool isWinning() const {
            return cell.isWinning();
        }
        bool isLocked() const {
            return (cell.isLocked());
        }

        void setShape(const TTT_Shape shape) {
            cell = shape;
        }

        bool playMove(const Path& path, TTT_Shape player);
        bool playMove(const Path& path, TTT_Shape player, int step, bool locked, Path currentPath = {});
        void undoMove(const Path& path, int step = 0);

        std::vector<Path> getAvailableMoves(const Path& target = {}, Path currentPath = {}) const;
        std::optional<Path> getRandomAvailableMove(const Path& target = {}, Path currentPath = {}) const;
};

} // namespace TTT

inline std::ostream& operator<<(std::ostream& os, const TTT::TTT_Shape& shape){
    os << TTT::TTT_Cell::ShapeToString(shape);
    return os;
};

inline std::ostream& operator<<(std::ostream& os, const TTT::TTT_Cell& cell){
    os << cell.ShapeToString();
    return os;
};

