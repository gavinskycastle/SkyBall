#include "../libs/raylib/src/raylib.h"
#include "player.hpp"
#include <iostream>

using namespace std;

    Footballer player;
void playerMovement() {
    while (!WindowShouldClose()) {
        player.DrawRectangle(player.x, player.y, player.length, player.width, RED);
        player.moveRectangle(player);
    }
}
