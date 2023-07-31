#include "lamp.state.hpp"
#include "../../logger.hpp"
#include "../../rx.hpp"

void LampState::enter() {
    Logger::getInstance().logLn("BootingState::enter() - starting boot animation");

    _ledController->setAnimation(&_animation);
    _ledController->setBrightness(255);

    _exitTask = this->_scheduler->every(200, [this]() {
        if (RX::ledBrightness > 0) {
            _stateMachine->toState(APP_STATE_ANIMATE);
        }
    });

    Logger::getInstance().logLn("BootingState::enter() - done");
}

void LampState::leave() {
    Logger::getInstance().logLn("BootingState::leave() - clearing animation");

    _ledController->clear();
    _exitTask->cancel();
    _scheduler->removeTask(_exitTask);

    Logger::getInstance().logLn("BootingState::leave() - done");
}