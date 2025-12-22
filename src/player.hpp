#ifndef PLAYER_H
#define PLAYER_H

#include "../libs/raylib/src/raylib.h"
#include <iostream>

using namespace std;

//Enums
enum PLAYER_STATE {
    IDLE,
    WALKING,
    RUNNING
};

//Main Class
class Player {
    public:
        float x = 250.0f;
        float y = GetScreenHeight() / 2.0f - 17.5f; // Centered vertically
        // width = horizontal size, length = vertical size
        float width = 30.0f;
        float length = 35.0f;
        // Movement speed in pixels per second
        float walkSpeed = 5.0f;
        float runSpeed = walkSpeed * 2.0f;
        
        void rectangleIdle(float delta);
        void rectangleWalk(float delta, Rectangle fieldBounds);
        void rectangleRun(float delta, Rectangle fieldBounds);
        void playerMovement(float delta, Rectangle fieldBounds);
};

#endif