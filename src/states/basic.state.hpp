#pragma once

#include <bluefairy.h>

#include "app-state.hpp"
#include "../led-controller.hpp"

class BasicState: public bluefairy::State {
    public:
        BasicState(
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler,
            LedController *ledController
        ) {
            this->_stateMachine = stateMachine;
            this->_scheduler = scheduler;
            this->_ledController = ledController;
        };

    protected:
        AppStateMachine *_stateMachine;
        bluefairy::Scheduler *_scheduler;
        LedController *_ledController;
};
