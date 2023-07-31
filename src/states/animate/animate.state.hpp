#pragma once

#include <bluefairy.h>

#include "../basic.state.hpp"
#include "../../animations/single-color-breathing-animation.hpp"
#include "../../animations/walking-rainbow-stick-reactive-animation.hpp"
#include "const.h"

enum AnimationType {
    ANIMATION_TYPE_OFF,
    ANIMATION_TYPE_ARMED,
    ANIMATION_TYPE_ARMED_HIGH_THROTTLE,
    ANIMATION_TYPE_DISARMED
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
        _disarmedAnimation(CHSV(0, 255, 255), 50, 200)
        {};

        void enter();
        void leave();
    
    private:
        WalkingRainbowStickReactiveAnimation _armedAnimation;
        SingleColorBreathingAnimation _disarmedAnimation;
        bluefairy::TaskNode *_updateTask;
        bluefairy::TaskNode *_lampTask;
        AnimationType _activeAnimation;

        void updateAnimation();
        void updateArmedAnimation();
};
