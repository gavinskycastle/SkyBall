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
        int x = 10;
        int y = 10;
        int length = 8;
        int width = 8;
        int moveSpeed = 1;
        
        void moveRectangle(float delta);
        void playerMovement(float delta);
};

#endif