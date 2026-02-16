#include "Renderer.hpp"
#include <algorithm>
#include <cstring>

Renderer::Renderer() : renderer_(nullptr), window_(nullptr), font_(nullptr) {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(SDL_Window* window) {
    window_ = window;
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        return false;
    }

    if (TTF_Init() < 0) {
        SDL_DestroyRenderer(renderer_);
        return false;
    }

    font_ = TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Regular.ttf", 16);
    if (!font_) {
        // Try alternative fonts
        font_ = TTF_OpenFont("/usr/share/fonts/TTF/OpenSans-Regular.ttf", 16);
        if (!font_) {
            font_ = TTF_OpenFont("/usr/share/fonts/noto/NotoSans-Regular.ttf", 16);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    return true;
}

void Renderer::shutdown() {
    if (font_) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }
    TTF_Quit();
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}

void Renderer::drawBoard(const Board& board) {
    // Draw border
    drawRect(GRID_OFFSET_X - 2, GRID_OFFSET_Y - 2, 
             Board::WIDTH * CELL_SIZE + 4, Board::HEIGHT * CELL_SIZE + 4,
             100, 100, 100);
    
    // Draw grid cells
    for (int y = 0; y < Board::HEIGHT; ++y) {
        for (int x = 0; x < Board::WIDTH; ++x) {
            int color = board.getCell(x, y + Board::HIDDEN_ROWS);
            drawCell(x, y, color, GRID_OFFSET_X, GRID_OFFSET_Y, false);
        }
    }
    
    // Draw grid lines
    SDL_SetRenderDrawColor(renderer_, 50, 50, 50, 255);
    for (int x = 0; x <= Board::WIDTH; ++x) {
        int lineX = GRID_OFFSET_X + x * CELL_SIZE;
        SDL_RenderDrawLine(renderer_, lineX, GRID_OFFSET_Y, 
                          lineX, GRID_OFFSET_Y + Board::HEIGHT * CELL_SIZE);
    }
    for (int y = 0; y <= Board::HEIGHT; ++y) {
        int lineY = GRID_OFFSET_Y + y * CELL_SIZE;
        SDL_RenderDrawLine(renderer_, GRID_OFFSET_X, lineY,
                          GRID_OFFSET_X + Board::WIDTH * CELL_SIZE, lineY);
    }
}

void Renderer::drawPiece(const Piece& piece, const Board& board, bool ghost) {
    auto blocks = piece.getBlocks();
    
    for (const auto& [bx, by] : blocks) {
        // Skip hidden rows
        if (by < Board::HIDDEN_ROWS) continue;
        
        int drawY = by - Board::HIDDEN_ROWS;
        drawCell(bx, drawY, piece.getColor(), GRID_OFFSET_X, GRID_OFFSET_Y, ghost);
    }
}

void Renderer::drawNextPiece(const Piece& piece) {
    // Draw preview box
    drawRect(PREVIEW_OFFSET_X - 5, PREVIEW_OFFSET_Y - 30, 120, 130, 80, 80, 80);
    drawText("NEXT", PREVIEW_OFFSET_X + 35, PREVIEW_OFFSET_Y - 25);
    
    // Center the piece in preview
    auto blocks = piece.getBlocks();
    if (blocks.empty()) return;
    
    int minX = 4, maxX = 0, minY = 4, maxY = 0;
    for (const auto& [bx, by] : blocks) {
        minX = std::min(minX, bx);
        maxX = std::max(maxX, bx);
        minY = std::min(minY, by);
        maxY = std::max(maxY, by);
    }
    
    int offsetX = (4 - (maxX - minX + 1)) / 2;
    int offsetY = (4 - (maxY - minY + 1)) / 2;
    
    for (const auto& [bx, by] : blocks) {
        int drawX = bx - minX + offsetX;
        int drawY = by - minY + offsetY;
        drawCell(drawX, drawY, piece.getColor(), 
                 PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y, false);
    }
}

void Renderer::drawUI(int score, int level, int lines) {
    char buffer[64];
    
    // Score
    snprintf(buffer, sizeof(buffer), "SCORE: %d", score);
    drawText(buffer, PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 150);
    
    // Level
    snprintf(buffer, sizeof(buffer), "LEVEL: %d", level);
    drawText(buffer, PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 180);
    
    // Lines
    snprintf(buffer, sizeof(buffer), "LINES: %d", lines);
    drawText(buffer, PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 210);
    
    // Controls
    drawText("CONTROLS:", PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 260);
    drawText("Left/Right: Move", PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 285);
    drawText("Down: Soft Drop", PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 305);
    drawText("Up/Z: Rotate", PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 325);
    drawText("Space: Hard Drop", PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 345);
    drawText("P: Pause", PREVIEW_OFFSET_X, PREVIEW_OFFSET_Y + 365);
}

void Renderer::drawGameOver() {
    // Semi-transparent overlay
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, 600, 700};
    SDL_RenderFillRect(renderer_, &overlay);
    
    drawText("GAME OVER", GRID_OFFSET_X + 60, 300);
    drawText("Press P to restart", GRID_OFFSET_X + 50, 340);
}

void Renderer::drawPaused() {
    // Semi-transparent overlay
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, 600, 700};
    SDL_RenderFillRect(renderer_, &overlay);
    
    drawText("PAUSED", GRID_OFFSET_X + 80, 300);
    drawText("Press P to resume", GRID_OFFSET_X + 40, 340);
}

void Renderer::drawCell(int x, int y, int color, int offsetX, int offsetY, bool ghost) {
    int drawX = offsetX + x * CELL_SIZE + 1;
    int drawY = offsetY + y * CELL_SIZE + 1;
    int size = CELL_SIZE - 2;
    
    auto [r, g, b] = colors_[color];
    
    if (ghost) {
        // Ghost piece is semi-transparent
        SDL_SetRenderDrawColor(renderer_, r, g, b, 80);
        SDL_Rect rect = {drawX, drawY, size, size};
        SDL_RenderFillRect(renderer_, &rect);
        
        SDL_SetRenderDrawColor(renderer_, r, g, b, 150);
        SDL_RenderDrawRect(renderer_, &rect);
    } else {
        // Normal piece with gradient effect
        SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
        SDL_Rect rect = {drawX, drawY, size, size};
        SDL_RenderFillRect(renderer_, &rect);
        
        // Highlight
        SDL_SetRenderDrawColor(renderer_, std::min(255, r + 40), std::min(255, g + 40), std::min(255, b + 40), 255);
        SDL_Rect highlight = {drawX, drawY, size, 3};
        SDL_RenderFillRect(renderer_, &highlight);
        
        // Shadow
        SDL_SetRenderDrawColor(renderer_, r * 0.7, g * 0.7, b * 0.7, 255);
        SDL_Rect shadow = {drawX, drawY + size - 3, size, 3};
        SDL_RenderFillRect(renderer_, &shadow);
    }
}

void Renderer::drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer_, &rect);
}

void Renderer::drawText(const char* text, int x, int y) {
    if (!font_) {
        // Fallback: draw simple rectangles if font not loaded
        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        int startX = x;
        for (const char* c = text; *c; ++c) {
            if (*c == ' ') {
                startX += 10;
                continue;
            }
            SDL_Rect charRect = {startX, y, 6, 10};
            SDL_RenderFillRect(renderer_, &charRect);
            startX += 10;
        }
        return;
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font_, text, white);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
        if (texture) {
            SDL_Rect dstRect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer_, texture, nullptr, &dstRect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}
