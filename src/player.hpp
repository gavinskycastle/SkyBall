#define PLAYER_F

#include "../libs/raylib/src/raylib.h"
#include <iostream>

using namespace std;

//Variables
int distance = 1;
float delta = GetFrameTime();
//Enums
enum PLAYER_STATE {
    IDLE,
    WALKING,
    RUNNING
};

//Prototype Functions
void moveRectangle();

//Main Class
class Footballer {
    private:
        int x = 10;
        int y = 10;
        int length = 8;
        int width = 8;

    public:
        DrawRectangle(int x, int y, int length, int width, Color GRAY);
        moveRectangle();
};


void moveRectangle() {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        Footballer.y += distance * delta;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        Footballer.x -= distance * delta;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        Footballer.y -= distance * delta;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        Footballer.x += distance * delta;
    }
}