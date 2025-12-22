#ifndef BALL_H
#define BALL_H

#include "../libs/raylib/src/raylib.h"
#include <string>
#include <vector>
#include "player.hpp"

enum BallState {
    IN_PLAY,
    SCORED,
    FALLEN
};

struct BallAssets {
    Model soccerBallModel;
    Vector3 soccerBallCenter;
    Sound ballKickSound;
    Sound goalSound;
    Sound fallSound;
};

class Ball {
    public:
        Ball(int x, int y, int round = 1);
        void init(BallAssets* ballAssets);
        void kick(float Fx, float Fy);
        void bounceBack(float Fx, float Fy);
        bool update(float relDt, std::vector<Player>& players, Rectangle fieldBounds, Rectangle goalBoundA, Rectangle goalBoundB);
        void close();
        
        BallState ballState = IN_PLAY;
        float velocityMultiplier = 0.5f; // increase after each kick
    private:
        const float friction = 0.995f;
        
        // rendering
        BallAssets* assets;
        
        Camera3D camera = { 0 };
        RenderTexture2D renderTexture;
        Vector3 ballModelPosition = Vector3{0.0f, -6.0f, 0.0f};
        int textureSize = 16;
        float xRotation = 0.0f;
        float zRotation = 0.0f;
        
        // ball state
        float x;
        float y;
        
        float vx = 0;
        float vy = 0;
        
        float Fx = 0;
        float Fy = 0;
};

#endif // BALL_H