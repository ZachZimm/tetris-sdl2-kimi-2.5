#include "Board.hpp"

Board::Board() : grid_(HEIGHT + HIDDEN_ROWS, std::vector<int>(WIDTH, 0)) {}

void Board::clear() {
    for (auto& row : grid_) {
        std::fill(row.begin(), row.end(), 0);
    }
}

int Board::getCell(int x, int y) const {
    if (!isValidPosition(x, y)) return -1;
    return grid_[y][x];
}

void Board::setCell(int x, int y, int value) {
    if (isValidPosition(x, y)) {
        grid_[y][x] = value;
    }
}

bool Board::isOccupied(int x, int y) const {
    if (x < 0 || x >= WIDTH || y >= HEIGHT + HIDDEN_ROWS) return true;
    if (y < 0) return false;
    return grid_[y][x] != 0;
}

bool Board::isValidPosition(int x, int y) const {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT + HIDDEN_ROWS;
}

int Board::clearLines() {
    int linesCleared = 0;
    for (int y = HEIGHT + HIDDEN_ROWS - 1; y >= 0; --y) {
        if (isLineComplete(y)) {
            removeLine(y);
            ++linesCleared;
            ++y; // Check same row again since we shifted down
        }
    }
    return linesCleared;
}

bool Board::isLineComplete(int y) const {
    for (int x = 0; x < WIDTH; ++x) {
        if (grid_[y][x] == 0) return false;
    }
    return true;
}

void Board::removeLine(int y) {
    for (int row = y; row > 0; --row) {
        grid_[row] = grid_[row - 1];
    }
    grid_[0] = std::vector<int>(WIDTH, 0);
}
