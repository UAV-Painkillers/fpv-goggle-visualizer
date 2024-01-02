#include "animate.state.hpp"
#include "../../rx.hpp"
#include "../../logger.hpp"

void AnimateState::enter() {
    Logger::getInstance().logLn("AnimateState::enter() - starting animation update task");

    _updateTask = _scheduler->every(25, [this]() {
        updateAnimation();
    });

    Logger::getInstance().logLn("AnimateState::enter() - done");
}

void AnimateState::updateArmedAnimation() {
    if (_activeAnimation != ANIMATION_TYPE_ARMED) {
        Logger::getInstance().logLn("AnimateState::updateAnimation() - setting armed animation");
        _ledController->setAnimation(&_armedAnimation);
        _activeAnimation = ANIMATION_TYPE_ARMED;
    }
}

void AnimateState::updateAnimation() {
    if (RX::sticksAreAtBottomInside && !RX::isArmed) {
        Logger::getInstance().logLn("AnimateState::updateAnimation() - going to AppState::APP_STATE_OTA");
        _stateMachine->toState(AppState::APP_STATE_OTA);
        return;
    }

    if (!RX::ledSwitchIsOn) {
        if (_activeAnimation != ANIMATION_TYPE_OFF) {
            Logger::getInstance().logLn("AnimateState::updateAnimation() - turning off LED");
            _ledController->clear();
            _activeAnimation = ANIMATION_TYPE_OFF;
        }

        return;
    }

    if (RX::isArmed) {
        updateArmedAnimation();
        return;
    }

    if (_activeAnimation != ANIMATION_TYPE_DISARMED) {
        Logger::getInstance().logLn("AnimateState::updateAnimation() - setting disarmed animation");
        _ledController->setAnimation(&_disarmedAnimation);
        _activeAnimation = ANIMATION_TYPE_DISARMED;
    }
}

void AnimateState::leave() {
    Logger::getInstance().logLn("AnimateState::leave() - cancelling animation update task");
    _updateTask->cancel();
    _scheduler->removeTask(_updateTask);

    _activeAnimation = ANIMATION_TYPE_OFF;

    Logger::getInstance().logLn("AnimateState::leave() - clearing animation");
    _ledController->clear();

    Logger::getInstance().logLn("AnimateState::leave() - done");
}