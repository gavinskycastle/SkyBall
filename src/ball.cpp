#include "../libs/raylib/src/raylib.h"
#include "../libs/raylib/src/rlgl.h"

#include "ball.hpp"

Ball::Ball(int x, int y) {
    this->x = x;
    this->y = y;
}

void Ball::init(std::string assetPathPrefix) {
    soccerBallModel = LoadModel((assetPathPrefix + "soccerBall/soccerBall.obj").c_str());
    BoundingBox soccerBallBox = GetModelBoundingBox(soccerBallModel);
    soccerBallCenter = Vector3{
        (soccerBallBox.min.x + ((soccerBallBox.max.x - soccerBallBox.min.x) / 2.0f))+ballModelPosition.x,
        (soccerBallBox.min.y + ((soccerBallBox.max.y - soccerBallBox.min.y) / 2.0f))+ballModelPosition.y,
        (soccerBallBox.min.z + ((soccerBallBox.max.z - soccerBallBox.min.z) / 2.0f))+ballModelPosition.z
    };
    
    camera.position = Vector3{ 18.0f, 0.0f, 0.0f }; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type
    
    renderTexture = LoadRenderTexture(textureSize, textureSize);
}

void Ball::kick(float aFx, float aFy) {
    Fx += aFx;
    Fy += aFy;
}

void Ball::update(float relDt) {
    // Update ball physics
    vx += Fx * relDt;
    vy += Fy * relDt;
    
    x += vx * relDt * velocityMultiplier;
    y += vy * relDt * velocityMultiplier;
    
    // Friction
    vx *= friction;
    vy *= friction;
    
    // Rotation
    xRotation += vx * ((float)textureSize / 180.0f);
    zRotation += vy * ((float)textureSize / 180.0f);
    
    // Keep ball within bounds
    if (x > 720) {
        x = 720.0f;
    }
    if (x < 0) {
        x = 0.0f;
    }
    if (y > 480) {
        y = 480.0f;
    }
    if (y < 0) {
        y = 0.0f;
    }
    
    // 3D rendering
    UpdateCamera(&camera, CAMERA_CUSTOM);
    
    BeginTextureMode(renderTexture);
        ClearBackground(BLANK);
        BeginMode3D(camera);
            rlPushMatrix();

            // Translate to the origin relative to the soccer ball's center
            rlTranslatef(soccerBallCenter.x, soccerBallCenter.y, soccerBallCenter.z);

            // Apply rotation around the soccer ball's center
            rlRotatef(xRotation, 0.0f, 1.0f, 0.0f);
            
            rlRotatef(zRotation, 0.0f, 0.0f, 1.0f);

            // Translate back to the original position
            rlTranslatef(-soccerBallCenter.x, -soccerBallCenter.y, -soccerBallCenter.z);
            
            // Draw the soccer ball model
            DrawModel(soccerBallModel, ballModelPosition, 1.0f, WHITE);
            
            rlPopMatrix();
        EndMode3D();
    EndTextureMode();
    
    DrawTexturePro(renderTexture.texture, Rectangle{0, 0, (float)textureSize, (float)textureSize}, Rectangle{x-textureSize, y-textureSize, (float)(textureSize * 2), (float)(textureSize * 2)}, Vector2{0, 0}, 0.0f, WHITE);
}