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

RenderTexture2D Ball::update(float relDt) {
    // static float rotationAngle = 0.0f;
    // GuiSliderBar(Rectangle{170, 10, 200, 20}, NULL, NULL, &rotationAngle, -180.0f, 180.0f);
    // GuiLabel(Rectangle{10, 10, 250, 15}, ("Angle " + std::to_string(rotationAngle)).c_str());
    
    xRotation += 2.0 * relDt;
    
    // 3D rendering
    UpdateCamera(&camera, CAMERA_CUSTOM);
    
    BeginTextureMode(renderTexture);
        ClearBackground(BLANK);
        BeginMode3D(camera);
            rlPushMatrix();

            // Translate to the origin relative to the soccer ball's center
            rlTranslatef(soccerBallCenter.x, soccerBallCenter.y, soccerBallCenter.z);

            // Apply rotation around the soccer ball's center
            rlRotatef(xRotation, 1.0f, 0.0f, 0.0f);

            // Translate back to the original position
            rlTranslatef(-soccerBallCenter.x, -soccerBallCenter.y, -soccerBallCenter.z);
            
            // Draw the soccer ball model
            DrawModel(soccerBallModel, ballModelPosition, 1.0f, WHITE);
            
            rlPopMatrix();
        EndMode3D();
    EndTextureMode();
    
    return renderTexture;
}