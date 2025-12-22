#include "../libs/raylib/src/raylib.h"
#include "../libs/raylib/src/rlgl.h"

#include "ball.hpp"

Ball::Ball(int x, int y, int round) {
    this->x = x;
    this->y = y;
    
    this->velocityMultiplier = 0.5f + (round - 1) * 0.1f;
    
    static Model soccerBallModel;
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
    static float lastKickTime = 0.0f; // Tracks the time of the last kick
    float currentTime = GetTime();   // Get the current time

    if (currentTime - lastKickTime >= 0.25f) { // Check if 0.25 seconds have passed
        Fx += aFx;
        Fy += aFy;
        lastKickTime = currentTime; // Update the last kick time
        
        velocityMultiplier += 0.05f;
        //std::cout << "Velocity Multiplier: " << velocityMultiplier << std::endl;
    }
}

void Ball::bounceBack(float bFx, float bFy) {
    const float damping = 0.95f; // reduce velocity slightly on bounce

    if (x > (float)GetScreenWidth()) {
        x = (float)GetScreenWidth();
        vx = -vx * damping;
        Fx = -Fx * damping;
    }
    if (x < 0.0f) {
        x = 0.0f;
        vx = -vx * damping;
        Fx = -Fx * damping;
    }

    if (y > (float)GetScreenHeight()) {
        y = (float)GetScreenHeight();
        vy = -vy * damping;
        Fy = -Fy * damping;
    }
    if (y < 0.0f) {
        y = 0.0f;
        vy = -vy * damping;
        Fy = -Fy * damping;
    }

    // Optional: apply any external bounce forces passed in
    Fx += bFx;
    Fy += bFy;

}

bool Ball::update(float relDt, std::vector<Player>& players, Rectangle fieldBounds, Rectangle goalBoundA, Rectangle goalBoundB) {
    bool stateChanged = false;
    
    Rectangle ballRect = Rectangle{x-textureSize, y-textureSize, (float)(textureSize * 2), (float)(textureSize * 2)};
    
    for (Player& player : players) {
        Rectangle playerRect = Rectangle{player.x, player.y, player.width, player.length};
        if (CheckCollisionRecs(ballRect, playerRect)) {
            kick((vx - (player.x + player.width / 2.0f - x)) * 0.15f, (vy - (player.y + player.length / 2.0f - y)) * 0.15f);
        }
    }
    
    if (ballState == IN_PLAY) {
        if (CheckCollisionRecs(ballRect, goalBoundA) || CheckCollisionRecs(ballRect, goalBoundB)) {
            ballState = SCORED;
            stateChanged = true;
        } else if (!CheckCollisionRecs(ballRect, fieldBounds)) {
            ballState = FALLEN;
            stateChanged = true;
        }
    } else if (ballState == SCORED) {
        // If the y value of the ball is above or below either goal, bring it back inside the goal
        if (y < goalBoundA.y) {
            y = goalBoundA.y;
        } else if (y > goalBoundA.y + goalBoundA.height - textureSize) {
            y = goalBoundA.y + goalBoundA.height - textureSize;
        }
    } else if (ballState == FALLEN) {
        Fx = 0;
        Fy = 0;
        vx = 0;
        vy = 0;
        camera.position = Vector3{camera.position.x + 0.2f * relDt, camera.position.y, camera.position.z};
    }
    
    // Update ball physics
    vx += Fx * relDt;
    vy += Fy * relDt;
    
    x += vx * relDt * velocityMultiplier;
    y += vy * relDt * velocityMultiplier;
    
    // Friction
    Fx *= friction;
    Fy *= friction;
    if (std::abs(Fx) < 0.001f) {
        vx *= friction;
    }
    if (std::abs(Fy) < 0.001f) {
        vy *= friction;
    }

    // Rotation
    xRotation += vx * ((float)textureSize / 180.0f);
    zRotation += vy * ((float)textureSize / 180.0f);

    // Handle wall collisions (invert velocity when hitting edges)
    bounceBack(0.0f, 0.0f);
    
    // Keep ball within bounds
    if (x > GetScreenWidth()) {
        x = GetScreenWidth();
    }
    if (x < 0) {
        x = 0.0f;
    }
    if (y > GetScreenHeight()) {
        y = GetScreenHeight();
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
    
    DrawTexturePro(renderTexture.texture, Rectangle{0, 0, (float)textureSize, (float)textureSize}, ballRect, Vector2{0, 0}, 0.0f, WHITE);
    
    // DrawText(("X Velocity: " + std::to_string(vx)).c_str(), 10, 10, 20, BLACK);
    // DrawText(("Y Velocity: " + std::to_string(vy)).c_str(), 10, 30, 20, BLACK);
    // DrawText(("X Force: " + std::to_string(Fx)).c_str(), 10, 50, 20, BLACK);
    // DrawText(("Y Force: " + std::to_string(Fy)).c_str(), 10, 70, 20, BLACK);
    
    return stateChanged;
}