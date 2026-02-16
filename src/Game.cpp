#include "Game.hpp"
#include "Renderer.hpp"
#include "InputHandler.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include <SDL2/SDL.h>
#include <algorithm>

Game::Game()
    : window_(nullptr)
    , running_(false)
    , state_(GameState::PLAYING)
    , board_(Board::HEIGHT + Board::HIDDEN_ROWS, std::vector<int>(Board::WIDTH, 0))
    , score_(0)
    , level_(1)
    , linesCleared_(0)
    , fallTimer_(0.0f)
    , fallSpeed_(INITIAL_FALL_SPEED) {}

Game::~Game() {
    shutdown();
}

bool Game::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        return false;
    }
    
    window_ = SDL_CreateWindow("Tetris",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              600, 700, SDL_WINDOW_SHOWN);
    if (!window_) {
        return false;
    }
    
    renderer_ = std::make_unique<Renderer>();
    if (!renderer_->initialize(window_)) {
        return false;
    }
    
    inputHandler_ = std::make_unique<InputHandler>();
    
    // Initialize game state
    for (auto& row : board_) {
        std::fill(row.begin(), row.end(), 0);
    }
    nextPiece_ = std::make_unique<Piece>(Piece::createRandom());
    spawnPiece();
    
    running_ = true;
    return true;
}

void Game::shutdown() {
    renderer_.reset();
    inputHandler_.reset();
    currentPiece_.reset();
    nextPiece_.reset();
    
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    SDL_Quit();
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    
    while (running_) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        processInput();
        
        if (state_ == GameState::PLAYING) {
            update(deltaTime);
        }
        
        render();
        
        // Cap at ~60 FPS
        SDL_Delay(16);
    }
}

void Game::processInput() {
    inputHandler_->update();
    
    if (inputHandler_->shouldQuit()) {
        running_ = false;
        return;
    }
    
    InputAction action = inputHandler_->getAction();
    
    switch (action) {
        case InputAction::PAUSE:
            if (state_ == GameState::GAME_OVER) {
                // Restart game
                for (auto& row : board_) {
                    std::fill(row.begin(), row.end(), 0);
                }
                score_ = 0;
                level_ = 1;
                linesCleared_ = 0;
                fallSpeed_ = INITIAL_FALL_SPEED;
                nextPiece_ = std::make_unique<Piece>(Piece::createRandom());
                spawnPiece();
                state_ = GameState::PLAYING;
            } else {
                state_ = (state_ == GameState::PAUSED) ? GameState::PLAYING : GameState::PAUSED;
            }
            inputHandler_->resetAction();
            break;
            
        default:
            break;
    }
    
    if (state_ != GameState::PLAYING) return;
    
    // Handle piece movement
    switch (action) {
        case InputAction::MOVE_LEFT:
            if (currentPiece_) {
                currentPiece_->move(-1, 0);
                if (!canPlacePiece(*currentPiece_)) {
                    currentPiece_->move(1, 0);
                }
            }
            break;
            
        case InputAction::MOVE_RIGHT:
            if (currentPiece_) {
                currentPiece_->move(1, 0);
                if (!canPlacePiece(*currentPiece_)) {
                    currentPiece_->move(-1, 0);
                }
            }
            break;
            
        case InputAction::MOVE_DOWN:
            if (currentPiece_) {
                currentPiece_->move(0, 1);
                if (!canPlacePiece(*currentPiece_)) {
                    currentPiece_->move(0, -1);
                    lockPiece();
                }
                score_ += 1;
            }
            break;
            
        case InputAction::ROTATE_CW:
            if (currentPiece_) {
                currentPiece_->rotate(1);
                if (!canPlacePiece(*currentPiece_)) {
                    // Try wall kicks
                    currentPiece_->move(-1, 0);
                    if (!canPlacePiece(*currentPiece_)) {
                        currentPiece_->move(2, 0);
                        if (!canPlacePiece(*currentPiece_)) {
                            currentPiece_->move(-1, 0);
                            currentPiece_->rotate(-1);
                        }
                    }
                }
            }
            break;
            
        case InputAction::ROTATE_CCW:
            if (currentPiece_) {
                currentPiece_->rotate(-1);
                if (!canPlacePiece(*currentPiece_)) {
                    currentPiece_->move(-1, 0);
                    if (!canPlacePiece(*currentPiece_)) {
                        currentPiece_->move(2, 0);
                        if (!canPlacePiece(*currentPiece_)) {
                            currentPiece_->move(-1, 0);
                            currentPiece_->rotate(1);
                        }
                    }
                }
            }
            break;
            
        case InputAction::HARD_DROP:
            if (currentPiece_) {
                while (canPlacePiece(*currentPiece_)) {
                    currentPiece_->move(0, 1);
                    score_ += 2;
                }
                currentPiece_->move(0, -1);
                lockPiece();
            }
            break;
            
        default:
            break;
    }
}

void Game::update(float deltaTime) {
    if (!currentPiece_) return;
    
    fallTimer_ += deltaTime;
    
    if (fallTimer_ >= fallSpeed_) {
        fallTimer_ = 0.0f;
        
        currentPiece_->move(0, 1);
        
        if (!canPlacePiece(*currentPiece_)) {
            currentPiece_->move(0, -1);
            lockPiece();
        }
    }
}

void Game::render() {
    renderer_->clear();
    
    // Convert board to old format for renderer
    Board board;
    for (int y = 0; y < Board::HEIGHT + Board::HIDDEN_ROWS; ++y) {
        for (int x = 0; x < Board::WIDTH; ++x) {
            board.setCell(x, y, board_[y][x]);
        }
    }
    
    renderer_->drawBoard(board);
    
    // Draw ghost piece
    if (currentPiece_ && state_ == GameState::PLAYING) {
        Piece ghost = *currentPiece_;
        while (canPlacePiece(ghost)) {
            ghost.move(0, 1);
        }
        ghost.move(0, -1);
        renderer_->drawPiece(ghost, board, true);
    }
    
    // Draw current piece
    if (currentPiece_) {
        renderer_->drawPiece(*currentPiece_, board, false);
    }
    
    // Draw next piece
    if (nextPiece_) {
        renderer_->drawNextPiece(*nextPiece_);
    }
    
    // Draw UI
    renderer_->drawUI(score_, level_, linesCleared_);
    
    // Draw overlays
    if (state_ == GameState::GAME_OVER) {
        renderer_->drawGameOver();
    } else if (state_ == GameState::PAUSED) {
        renderer_->drawPaused();
    }
    
    renderer_->present();
}

void Game::spawnPiece() {
    currentPiece_ = std::move(nextPiece_);
    nextPiece_ = std::make_unique<Piece>(Piece::createRandom());
    
    // Check if game over
    if (!canPlacePiece(*currentPiece_)) {
        state_ = GameState::GAME_OVER;
    }
}

void Game::lockPiece() {
    if (!currentPiece_) return;
    
    auto blocks = currentPiece_->getBlocks();
    for (const auto& [bx, by] : blocks) {
        if (by >= 0 && by < Board::HEIGHT + Board::HIDDEN_ROWS &&
            bx >= 0 && bx < Board::WIDTH) {
            board_[by][bx] = currentPiece_->getColor();
        }
    }
    
    // Clear lines
    int lines = clearLines();
    if (lines > 0) {
        updateScore(lines);
        updateLevel();
    }
    
    spawnPiece();
}

int Game::clearLines() {
    int linesCleared = 0;
    int y = Board::HEIGHT + Board::HIDDEN_ROWS - 1;
    while (y >= 0) {
        bool complete = true;
        for (int x = 0; x < Board::WIDTH; ++x) {
            if (board_[y][x] == 0) {
                complete = false;
                break;
            }
        }
        
        if (complete) {
            // Remove line
            for (int row = y; row > 0; --row) {
                board_[row] = board_[row - 1];
            }
            board_[0] = std::vector<int>(Board::WIDTH, 0);
            ++linesCleared;
            // Don't decrement y - check same row again since rows shifted down
        } else {
            --y;
        }
    }
    return linesCleared;
}

void Game::updateScore(int lines) {
    static const int lineScores[] = {0, 100, 300, 500, 800};
    score_ += lineScores[lines] * level_;
    linesCleared_ += lines;
}

void Game::updateLevel() {
    int newLevel = 1 + linesCleared_ / 10;
    if (newLevel > level_) {
        level_ = newLevel;
        fallSpeed_ = std::max(0.1f, INITIAL_FALL_SPEED - (level_ - 1) * SPEED_INCREMENT);
    }
}

bool Game::canPlacePiece(const Piece& piece) {
    auto blocks = piece.getBlocks();
    for (const auto& [bx, by] : blocks) {
        if (bx < 0 || bx >= Board::WIDTH || by >= Board::HEIGHT + Board::HIDDEN_ROWS) {
            return false;
        }
        if (by >= 0 && board_[by][bx] != 0) {
            return false;
        }
    }
    return true;
}
