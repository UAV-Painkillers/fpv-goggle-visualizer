#pragma once

#include <bluefairy.h>

#include "../basic.state.hpp"
#include "../../animations/rainbow-animation.hpp"
#include "../../animations/single-color-breathing-animation.hpp"
#include "const.h"

enum AnimationType {
    ANIMATION_TYPE_OFF,
    ANIMATION_TYPE_ARMED,
    ANIMATION_TYPE_DISARMED,
};

class AnimateState: public BasicState {
    public:
        AnimateState(
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler,
            LedController *ledController
        ): BasicState(
            stateMachine,
            scheduler,
            ledController
        ),
        _armedAnimation(),
        _disarmedAnimation(CHSV(0, 255, 255), 50, 3000)
        {};

        void enter();
        void leave();
    
    private:
        RainbowAnimation _armedAnimation;
        SingleColorBreathingAnimation _disarmedAnimation;
        bluefairy::TaskNode *_updateTask;
        AnimationType _activeAnimation;

        void updateAnimation();
};
