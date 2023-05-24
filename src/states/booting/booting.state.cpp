#include "booting.state.hpp"
#include "../../logger.hpp"

void BootingState::enter() {
    Logger::getInstance().logLn("BootingState::enter() - starting boot animation");

    _ledController->setAnimation(&_animation);

    uint8_t cycles = 0;
    _animation.onCompleted([this]() {
        Logger::getInstance().logLn("BootingState::enter() - animation cycle completed");
        Logger::getInstance().logLn("BootingState::enter() - going to AppState::APP_STATE_ANIMATE");
        _stateMachine->toState(AppState::APP_STATE_ANIMATE);
    });

    Logger::getInstance().logLn("BootingState::enter() - done");
}

void BootingState::leave() {
    Logger::getInstance().logLn("BootingState::leave() - clearing animation");

    _ledController->clear();

    Logger::getInstance().logLn("BootingState::leave() - done");
}