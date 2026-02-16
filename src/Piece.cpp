#include "Piece.hpp"
#include <random>

// Tetromino rotation data [PieceType][rotation][y][x]
const std::array<std::array<std::array<std::array<bool, 4>, 4>, 4>, 7> Piece::rotations_ = {{
    // I-Piece (Cyan)
    {{
        {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}},
        {{{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}},
        {{{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}}},
        {{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}}
    }},
    // O-Piece (Yellow)
    {{
        {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
        {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
        {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
        {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}}
    }},
    // T-Piece (Purple)
    {{
        {{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}},
        {{{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}},
        {{{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}}},
        {{{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}}
    }},
    // S-Piece (Green)
    {{
        {{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}},
        {{{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}},
        {{{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}}},
        {{{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}}
    }},
    // Z-Piece (Red)
    {{
        {{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
        {{{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}},
        {{{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}}},
        {{{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}}
    }},
    // J-Piece (Blue)
    {{
        {{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}},
        {{{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}},
        {{{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}}},
        {{{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}}
    }},
    // L-Piece (Orange)
    {{
        {{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}},
        {{{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}}},
        {{{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}}},
        {{{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}}
    }}
}};

Piece::Piece() : type_(PieceType::NONE), x_(0), y_(0), rotation_(0) {}

Piece::Piece(PieceType type) : type_(type), x_(4), y_(0), rotation_(0) {}

Piece Piece::createRandom() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 6);
    return Piece(static_cast<PieceType>(dis(gen)));
}

void Piece::move(int dx, int dy) {
    x_ += dx;
    y_ += dy;
}

void Piece::rotate(int direction) {
    rotation_ = (rotation_ + direction + 4) % 4;
}

std::vector<std::pair<int, int>> Piece::getBlocks() const {
    std::vector<std::pair<int, int>> blocks;
    if (type_ == PieceType::NONE) return blocks;
    
    int typeIdx = static_cast<int>(type_);
    for (int by = 0; by < BLOCK_SIZE; ++by) {
        for (int bx = 0; bx < BLOCK_SIZE; ++bx) {
            if (rotations_[typeIdx][rotation_][by][bx]) {
                blocks.emplace_back(x_ + bx, y_ + by);
            }
        }
    }
    return blocks;
}

int Piece::getColor() const {
    if (type_ == PieceType::NONE) return 0;
    return static_cast<int>(type_) + 1;
}
