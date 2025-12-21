#ifndef BALL_H
#define BALL_H

#include "../libs/raylib/src/raylib.h"

class Ball {
    public:
        Ball(int x, int y);
        void kick(float aFx, float aFy, float aFz);
        void update();
        void close();
    private:
        float x;
        float y;
        float z = 0;
        
        float Fx = 0;
        float Fy = 0;
        float Fz = 0;
        
        float velocityMultiplier;

};

#endif // BALL_H