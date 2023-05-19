#include "booting.state.hpp"
#include "../../logger.hpp"

void BootingState::enter() {
    Logger::logLn("BootingState::enter() - starting boot animation");

    _ledController->setAnimation(&_animation);

    _scheduler->timeout(
        BOOT_ANIMATION_DURATION_MS,
        [this]() {
            Logger::logLn("BootingState::enter() - going to AppState::APP_STATE_ANIMATE");
            _stateMachine->toState(AppState::APP_STATE_ANIMATE);
        }
    );

    Logger::logLn("BootingState::enter() - done");
}

void BootingState::leave() {
    Logger::logLn("BootingState::leave() - clearing animation");

    _ledController->clear();

    Logger::logLn("BootingState::leave() - done");
}