#ifndef BALL_H
#define BALL_H

#include "../libs/raylib/src/raylib.h"
#include <string>

class Ball {
    public:
        Ball(int x, int y);
        void init(std::string assetPathPrefix);
        void kick(float aFx, float aFy, float aFz);
        RenderTexture2D update(float relDt);
        void close();
    private:
        // rendering
        Model soccerBallModel;
        Vector3 soccerBallCenter;
        
        Camera3D camera = { 0 };
        RenderTexture2D renderTexture;
        Vector3 ballModelPosition = Vector3{0.0f, -6.0f, 0.0f};
        int textureSize = 40;
        float xRotation = 0.0f;
        float yRotation = 0.0f;
        
        // ball state
        float x;
        float y;
        float z = 0;
        
        float Fx = 0;
        float Fy = 0;
        float Fz = 0;
        
        float velocityMultiplier;

};

#endif // BALL_H