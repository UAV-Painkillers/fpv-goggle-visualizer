#pragma once

#include <bluefairy.h>

#include "../basic.state.hpp"
#include "../../animations/running-dot-animation.hpp"
#include "const.h"

class BootingState: public BasicState {
    public:
        BootingState(
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler,
            LedController *ledController
        ): BasicState(
            stateMachine,
            scheduler,
            ledController
        ), _animation(BOOT_ANIMATION_DURATION_MS / 2, CRGB::Red, CRGB::Black, 2) {};

        void enter();
        void leave();
    
    private:
        RunningDotAnimation _animation;
};
