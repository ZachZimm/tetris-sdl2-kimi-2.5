#include "InputHandler.hpp"

InputHandler::InputHandler()
    : currentAction_(InputAction::NONE)
    , quitRequested_(false)
    , leftPressed_(false)
    , rightPressed_(false)
    , downPressed_(false)
    , rotatePressed_(false)
    , lastRepeatTime_(0) {}

void InputHandler::update() {
    currentAction_ = InputAction::NONE;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quitRequested_ = true;
                break;
                
            case SDL_KEYDOWN:
                if (!event.key.repeat) {
                    switch (event.key.keysym.sym) {
                        case SDLK_LEFT:
                        case SDLK_a:
                            leftPressed_ = true;
                            currentAction_ = InputAction::MOVE_LEFT;
                            lastRepeatTime_ = SDL_GetTicks();
                            break;
                        case SDLK_RIGHT:
                        case SDLK_d:
                            rightPressed_ = true;
                            currentAction_ = InputAction::MOVE_RIGHT;
                            lastRepeatTime_ = SDL_GetTicks();
                            break;
                        case SDLK_DOWN:
                        case SDLK_s:
                            downPressed_ = true;
                            currentAction_ = InputAction::MOVE_DOWN;
                            break;
                        case SDLK_UP:
                        case SDLK_w:
                        case SDLK_x:
                            rotatePressed_ = true;
                            currentAction_ = InputAction::ROTATE_CW;
                            break;
                        case SDLK_z:
                            currentAction_ = InputAction::ROTATE_CCW;
                            break;
                        case SDLK_SPACE:
                            currentAction_ = InputAction::HARD_DROP;
                            break;
                        case SDLK_p:
                            currentAction_ = InputAction::PAUSE;
                            break;
                        case SDLK_ESCAPE:
                            quitRequested_ = true;
                            break;
                    }
                }
                break;
                
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_a:
                        leftPressed_ = false;
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                        rightPressed_ = false;
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:
                        downPressed_ = false;
                        break;
                    case SDLK_UP:
                    case SDLK_w:
                    case SDLK_x:
                        rotatePressed_ = false;
                        break;
                }
                break;
        }
    }
    
    // Handle auto-repeat for left/right
    Uint32 currentTime = SDL_GetTicks();
    if ((leftPressed_ || rightPressed_) && currentTime - lastRepeatTime_ > REPEAT_DELAY) {
        if (currentTime - lastRepeatTime_ > REPEAT_INTERVAL) {
            if (leftPressed_) {
                currentAction_ = InputAction::MOVE_LEFT;
            } else {
                currentAction_ = InputAction::MOVE_RIGHT;
            }
            lastRepeatTime_ = currentTime;
        }
    }
}

bool InputHandler::shouldQuit() const {
    return quitRequested_;
}

InputAction InputHandler::getAction() const {
    return currentAction_;
}

void InputHandler::resetAction() {
    currentAction_ = InputAction::NONE;
}
