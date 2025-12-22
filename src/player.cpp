#include "../libs/raylib/src/raylib.h"
#include "player.hpp"
#include <iostream>

using namespace std;

void Player::rectangleIdle(float delta) {
    // Draw the player rectangle in idle state
    DrawRectangle((int)x, (int)y, (int)width, (int)length, RED);
}

void Player::rectangleWalk(float delta, Rectangle fieldBounds) {
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
    if (x < fieldBounds.x) x = fieldBounds.x;
    if (y < fieldBounds.y) y = fieldBounds.y;
    if (x + width > fieldBounds.x + fieldBounds.width) x = fieldBounds.x + fieldBounds.width - width;
    if (y + length > fieldBounds.y + fieldBounds.height) y = fieldBounds.y + fieldBounds.height - length;
}

void Player::rectangleRun(float delta, Rectangle fieldBounds) {
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
    // Clamp position to field bounds
    if (x < fieldBounds.x) x = fieldBounds.x;
    if (y < fieldBounds.y) y = fieldBounds.y;
    if (x + width > fieldBounds.x + fieldBounds.width) x = fieldBounds.x + fieldBounds.width - width;
    if (y + length > fieldBounds.y + fieldBounds.height) y = fieldBounds.y + fieldBounds.height - length;
}

void Player::playerMovement(float delta, Rectangle fieldBounds) {
    DrawRectangle((int)x, (int)y, (int)width, (int)length, RED);
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        rectangleRun(delta, fieldBounds);
    } else {
        rectangleWalk(delta, fieldBounds);
    }
}