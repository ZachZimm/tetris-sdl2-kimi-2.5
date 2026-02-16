#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <memory>

class Piece;
class Renderer;
class InputHandler;

enum class GameState {
    PLAYING,
    PAUSED,
    GAME_OVER
};

class Game {
public:
    Game();
    ~Game();
    
    bool initialize();
    void run();
    void shutdown();
    
private:
    void processInput();
    void update(float deltaTime);
    void render();
    
    void spawnPiece();
    void lockPiece();
    int clearLines();
    void updateScore(int linesCleared);
    void updateLevel();
    bool canPlacePiece(const Piece& piece);
    
    SDL_Window* window_;
    bool running_;
    GameState state_;
    
    static constexpr int BOARD_WIDTH = 10;
    static constexpr int BOARD_HEIGHT = 20;
    
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<InputHandler> inputHandler_;
    
    std::vector<std::vector<int>> board_;
    std::unique_ptr<Piece> currentPiece_;
    std::unique_ptr<Piece> nextPiece_;
    
    int score_;
    int level_;
    int linesCleared_;
    
    float fallTimer_;
    float fallSpeed_;
    
    static constexpr float INITIAL_FALL_SPEED = 1.0f;
    static constexpr float SPEED_INCREMENT = 0.1f;
};
