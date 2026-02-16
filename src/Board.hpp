#pragma once

#include <vector>
#include <array>

class Board {
public:
    static constexpr int WIDTH = 10;
    static constexpr int HEIGHT = 20;
    static constexpr int HIDDEN_ROWS = 2;
    
    Board();
    
    void clear();
    int getCell(int x, int y) const;
    void setCell(int x, int y, int value);
    bool isOccupied(int x, int y) const;
    bool isValidPosition(int x, int y) const;
    
    int clearLines();
    bool isLineComplete(int y) const;
    void removeLine(int y);
    
private:
    std::vector<std::vector<int>> grid_;
};
