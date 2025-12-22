#include "../libs/raylib/src/raylib.h"
#include "player.hpp"
#include <iostream>

using namespace std;

void Player::moveRectangle(float delta) {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        y += moveSpeed * delta;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        x -= moveSpeed * delta;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        y -= moveSpeed * delta;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        x += moveSpeed * delta;
    }
}

void Player::playerMovement(float delta) {
    DrawRectangle(x, y, length, width, RED);
    moveRectangle(delta);
}