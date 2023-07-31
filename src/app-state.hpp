#pragma once

#include <bluefairy.h>

enum AppState {
    APP_STATE_BOOTING = 0,
    APP_STATE_OTA = 1,
    APP_STATE_ANIMATE = 2,
    APP_STATE_LAMP = 3
};

typedef bluefairy::StateMachine<4> AppStateMachine;