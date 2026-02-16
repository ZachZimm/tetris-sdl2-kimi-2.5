#pragma once

#include <SDL2/SDL.h>

enum class InputAction {
    NONE,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_DOWN,
    ROTATE_CW,
    ROTATE_CCW,
    HARD_DROP,
    PAUSE,
    QUIT
};

class InputHandler {
public:
    InputHandler();
    
    void update();
    
    bool shouldQuit() const;
    InputAction getAction() const;
    
    bool isLeftPressed() const { return leftPressed_; }
    bool isRightPressed() const { return rightPressed_; }
    bool isDownPressed() const { return downPressed_; }
    bool isRotatePressed() const { return rotatePressed_; }
    
    void resetAction();
    
private:
    InputAction currentAction_;
    bool quitRequested_;
    
    bool leftPressed_;
    bool rightPressed_;
    bool downPressed_;
    bool rotatePressed_;
    
    Uint32 lastRepeatTime_;
    static constexpr Uint32 REPEAT_DELAY = 150;
    static constexpr Uint32 REPEAT_INTERVAL = 50;
};
