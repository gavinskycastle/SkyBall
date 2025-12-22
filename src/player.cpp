#include "../libs/raylib/src/raylib.h"
#include "player.hpp"
#include <iostream>

using namespace std;

void Player::rectangleIdle(float delta) {
    // Draw the player rectangle in idle state
    DrawRectangle((int)x, (int)y, (int)width, (int)length, RED);
}

void Player::rectangleWalk(float delta) {
    float speed = IsKeyDown(KEY_LEFT_SHIFT) ? walkSpeed * 1.5f : walkSpeed;
    
    if (IsKeyDown(KEY_W)) {
        y -= speed * delta;
    }
    if (IsKeyDown(KEY_A)) {
        x -= speed * delta;
    }
    if (IsKeyDown(KEY_S)) {
        y += speed * delta;
    }
    if (IsKeyDown(KEY_D)) {
        x += speed * delta;
    }
    // Clamp position to screen bounds
    if (x < 0.0f) x = 0.0f;
    if (y < 0.0f) y = 0.0f;
    if (x + width > (float)GetScreenWidth()) x = (float)GetScreenWidth() - width;
    if (y + length > (float)GetScreenHeight()) y = (float)GetScreenHeight() - length;
}

void Player::rectangleRun(float delta) {
    float speed = runSpeed;
    
    if (IsKeyDown(KEY_W)) {
        y -= speed * delta;
    }
    if (IsKeyDown(KEY_A)) {
        x -= speed * delta;
    }
    if (IsKeyDown(KEY_S)) {
        y += speed * delta;
    }
    if (IsKeyDown(KEY_D)) {
        x += speed * delta;
    }
    // Clamp position to screen bounds
    if (x < 0.0f) x = 0.0f;
    if (y < 0.0f) y = 0.0f;
    if (x + width > (float)GetScreenWidth()) x = (float)GetScreenWidth() - width;
    if (y + length > (float)GetScreenHeight()) y = (float)GetScreenHeight() - length;
}

void Player::playerMovement(float delta) {
    DrawRectangle((int)x, (int)y, (int)width, (int)length, RED);
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        rectangleRun(delta);
    } else {
        rectangleWalk(delta);
    }
}