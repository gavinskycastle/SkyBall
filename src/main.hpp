#include <string>
#include <vector>
#include "../libs/raylib/src/raylib.h"

#ifndef APP_HPP
#define APP_HPP

enum GameState {
    PLAY,
    GAME_OVER,
    MAIN_MENU,
    OPTIONS,
    LEADER_BOARD,
};

struct GameSettings {
    float sfxVolume = 1.0f;
    bool enableTouchscreenControls = false;
};

struct GameInstanceState {
    int player;
    // Game instance state setup
    GameState gameState = MAIN_MENU;
    GameState gameOverReturnState = PLAY;
    int score = 0;
};

// Load asset and initialized stuffs here
void init_app();

// The main loop (return false to end loop)
bool app_loop();

// Free up allocated memory
void deinit_app();

#endif