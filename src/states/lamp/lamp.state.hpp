#pragma once

#include <bluefairy.h>

#include "../basic.state.hpp"
#include "../../animations/solid-color-animation.hpp"
#include "const.h"

class LampState: public BasicState {
    public:
        LampState(
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler,
            LedController *ledController
        ): BasicState(
            stateMachine,
            scheduler,
            ledController
        ), _animation(CRGB::White) {};

        void enter();
        void leave();
    
    private:
        SolidColorAnimation _animation;
        bluefairy::TaskNode *_exitTask;
};
