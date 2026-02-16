#pragma once

#include <array>
#include <vector>

enum class PieceType {
    I = 0,
    O = 1,
    T = 2,
    S = 3,
    Z = 4,
    J = 5,
    L = 6,
    NONE = -1
};

class Piece {
public:
    Piece();
    explicit Piece(PieceType type);
    
    static Piece createRandom();
    
    PieceType getType() const { return type_; }
    int getX() const { return x_; }
    int getY() const { return y_; }
    int getRotation() const { return rotation_; }
    
    void setX(int x) { x_ = x; }
    void setY(int y) { y_ = y; }
    void setRotation(int rotation) { rotation_ = rotation % 4; }
    
    void move(int dx, int dy);
    void rotate(int direction); // 1 = clockwise, -1 = counter-clockwise
    
    std::vector<std::pair<int, int>> getBlocks() const;
    int getColor() const;
    
    static constexpr int BLOCK_SIZE = 4;
    
private:
    static const std::array<std::array<std::array<std::array<bool, 4>, 4>, 4>, 7> rotations_;
    
    PieceType type_;
    int x_;
    int y_;
    int rotation_;
};
