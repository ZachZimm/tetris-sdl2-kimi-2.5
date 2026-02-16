#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Board.hpp"
#include "Piece.hpp"

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize(SDL_Window* window);
    void shutdown();

    void clear();
    void present();

    void drawBoard(const Board& board);
    void drawPiece(const Piece& piece, const Board& board, bool ghost = false);
    void drawNextPiece(const Piece& piece);
    void drawUI(int score, int level, int lines);
    void drawGameOver();
    void drawPaused();

    static constexpr int CELL_SIZE = 30;
    static constexpr int GRID_OFFSET_X = 50;
    static constexpr int GRID_OFFSET_Y = 50;
    static constexpr int PREVIEW_OFFSET_X = 400;
    static constexpr int PREVIEW_OFFSET_Y = 100;

private:
    void drawCell(int x, int y, int color, int offsetX, int offsetY, bool ghost = false);
    void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);
    void drawText(const char* text, int x, int y);

    SDL_Renderer* renderer_;
    SDL_Window* window_;
    TTF_Font* font_;

    static constexpr std::array<std::tuple<Uint8, Uint8, Uint8>, 8> colors_ = {{
        {128, 128, 128},  // 0: Empty (Gray)
        {0, 255, 255},    // 1: I (Cyan)
        {255, 255, 0},    // 2: O (Yellow)
        {128, 0, 128},    // 3: T (Purple)
        {0, 255, 0},      // 4: S (Green)
        {255, 0, 0},      // 5: Z (Red)
        {0, 0, 255},      // 6: J (Blue)
        {255, 165, 0}     // 7: L (Orange)
    }};
};
