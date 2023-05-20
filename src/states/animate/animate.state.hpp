#pragma once

#include <bluefairy.h>

#include "../basic.state.hpp"
#include "../../animations/blink-animation.hpp"
#include "../../animations/single-color-breathing-animation.hpp"
#include "../../animations/stick-reactive-animation.hpp"
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
        // _armedAnimation(CRGB::Blue, CRGB::Red, 200),
        _armedAnimation(),
        _disarmedAnimation(CHSV(0, 255, 255), 50, 200)
        {};

        void enter();
        void leave();
    
    private:
        StickReactiveAnimation _armedAnimation;
        SingleColorBreathingAnimation _disarmedAnimation;
        bluefairy::TaskNode *_updateTask;
        AnimationType _activeAnimation;

        void updateAnimation();
};
