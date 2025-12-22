#define PLAYER_F

#include "../libs/raylib/src/raylib.h"
#include <iostream>

using namespace std;

//Variables

float delta = GetFrameTime();
//Enums
enum PLAYER_STATE {
    IDLE,
    WALKING,
    RUNNING
};

//Prototype Functions
int DrawRectangle();
void moveRectangle();

//Main Class
class Footballer {
    public:
        int x = 10;
        int y = 10;
        int length = 8;
        int width = 8;
        int moveSpeed = 1;

        int DrawRectangle(int x, int y, int length, int width, Color color);
        void moveRectangle();
};


void moveRectangle(Footballer &player) {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        player.y += player.moveSpeed * delta;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        player.x -= player.moveSpeed * delta;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        player.y -= player.moveSpeed * delta;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        player.x += player.moveSpeed * delta;
    }
}