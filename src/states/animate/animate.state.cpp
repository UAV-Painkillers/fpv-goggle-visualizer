#include "animate.state.hpp"
#include "../../rx.hpp"
#include "../../logger.hpp"

void AnimateState::enter() {
    Logger::logLn("AnimateState::enter() - starting animation update task");

    _updateTask = _scheduler->every(25, [this]() {
        updateAnimation();
    });

    Logger::logLn("AnimateState::enter() - done");
}

void AnimateState::updateAnimation() {
    if (RX::otaIsActive) {
        Logger::logLn("AnimateState::updateAnimation() - going to AppState::APP_STATE_OTA");
        _stateMachine->toState(AppState::APP_STATE_OTA);
        return;
    }

    if (!RX::ledSwitchIsOn) {
        if (_activeAnimation != ANIMATION_TYPE_OFF) {
            Logger::logLn("AnimateState::updateAnimation() - turning off LED");
            _ledController->clear();
            _activeAnimation = ANIMATION_TYPE_OFF;
        }

        return;
    }

    if (RX::isArmed) {
        if (_activeAnimation != ANIMATION_TYPE_ARMED) {
            Logger::logLn("AnimateState::updateAnimation() - setting armed animation");
            _ledController->setAnimation(&_armedAnimation);
            _activeAnimation = ANIMATION_TYPE_ARMED;
        }

        return;
    }

    if (_activeAnimation != ANIMATION_TYPE_DISARMED) {
        Logger::logLn("AnimateState::updateAnimation() - setting disarmed animation");
        _ledController->setAnimation(&_disarmedAnimation);
        _activeAnimation = ANIMATION_TYPE_DISARMED;
    }
}

void AnimateState::leave() {
    Logger::logLn("AnimateState::leave() - cancelling animation update task");
    _updateTask->cancel();
    _scheduler->removeTask(_updateTask);

    Logger::logLn("AnimateState::leave() - clearing animation");
    _ledController->clear();

    Logger::logLn("AnimateState::leave() - done");
}