#ifndef BALL_H
#define BALL_H

#include "../libs/raylib/src/raylib.h"
#include <string>

class Ball {
    public:
        Ball(int x, int y);
        void init(std::string assetPathPrefix);
        void kick(float Fx, float Fy);
        void update(float relDt);
        void close();
    private:
        const float friction = 0.15f;
        
        // rendering
        Model soccerBallModel;
        Vector3 soccerBallCenter;
        
        Camera3D camera = { 0 };
        RenderTexture2D renderTexture;
        Vector3 ballModelPosition = Vector3{0.0f, -6.0f, 0.0f};
        int textureSize = 20;
        float xRotation = 0.0f;
        float zRotation = 0.0f;
        
        // ball state
        float x;
        float y;
        
        float vx = 0;
        float vy = 0;
        
        float Fx = 0;
        float Fy = 0;
        
        float velocityMultiplier = 1.0f; // increase after each kick
};

#endif // BALL_H