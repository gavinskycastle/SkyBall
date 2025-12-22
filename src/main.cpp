#include "../libs/raylib/src/raylib.h"
#include "helper.hpp"

#define RAYGUI_IMPLEMENTATION
#include "../libs/raygui/src/raygui.h"

#include <filesystem>

#include "main.hpp"
#include "leaderboard.hpp"
#include <raymath.h>

// Window setup
int screenWidth = 720; // Sprites are 2x scaled, so this will be 360x240
const int screenHeight = 480;
bool windowShouldClose = false;

// Loading resources
std::string assetPathPrefix = "../assets/";

Texture2D fieldTexture;

Model soccerBallModel;

// Leaderboard/name entry setup
bool highScoreEditMode = false;
char highScoreName[17] = "";
std::vector<std::string> leaderboardNames;
std::vector<int> leaderboardScores;
GameState selectedLeaderboardMode;
int selectedLeaderboardIndex = 0;

// Global game state setup
GameSettings gameSettings;

RenderTexture2D mainRenderTexture;
GameInstanceState mainGameInstance;

void DrawTextCentered(const char* text, int posX, int posY, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, posX-(textWidth/2), posY, fontSize, color);
}

void ResetGame(GameInstanceState &gameInstance) {
    gameInstance.score = 0;
    gameInstance.camera.fovy = 45.0f;
    gameInstance.targetFov = 45.0f;
    gameInstance.camera.position = Vector3{ 10.0f, 0.0f, 0.0f }; // Camera position
    gameInstance.camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    gameInstance.camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    gameInstance.camera.fovy = 45.0f;                              // Camera field-of-view Y
    gameInstance.camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type
}

void loadLeaderboardData(std::vector<std::string> &leaderboardNames, std::vector<int> &leaderboardScores) {
    leaderboardNames = getNames();
    if (leaderboardNames.size() > 10) leaderboardNames.erase(leaderboardNames.begin()+10, leaderboardNames.end());
    leaderboardScores = getScores();
    if (leaderboardScores.size() > 10) leaderboardScores.erase(leaderboardScores.begin()+10, leaderboardScores.end());
}

void DrawGameOver(GameInstanceState &gameInstance) {
    DrawTextCentered("Game Over", screenWidth/2, screenHeight/2-75, 50, BLACK);
    DrawTextCentered(("Score: " + std::to_string(gameInstance.score)).c_str(), screenWidth/2, screenHeight/2-25, 25, BLACK);
    if (checkScore(gameInstance.score)) {
        DrawTextCentered("Enter your name to submit your high score", screenWidth/2, screenHeight/2+8, 16, BLACK);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
        if (GuiTextBox(Rectangle{static_cast<float>(screenWidth/2-60), static_cast<float>(screenHeight/2+32), 120.0f, 24.0f}, highScoreName, 16, highScoreEditMode)) {
            highScoreEditMode = !highScoreEditMode;
        }
        GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
        if (GuiButton(Rectangle {static_cast<float>(screenWidth/2-100), static_cast<float>(screenHeight/2+75), 200.0f, 50.0f}, "Play Again") == 1) {
            gameInstance.gameState = gameInstance.gameOverReturnState;
            selectLeaderboardMode(gameInstance.gameOverReturnState);
            saveScore(highScoreName, gameInstance.score);
            ResetGame(gameInstance);
        };
        if (GuiButton(Rectangle {static_cast<float>(screenWidth/2-100), screenHeight/2+135, 200, 50}, "Main Menu") == 1) {
            gameInstance.gameState = MAIN_MENU;
            selectLeaderboardMode(gameInstance.gameOverReturnState);
            saveScore(highScoreName, gameInstance.score);
            ResetGame(gameInstance);
        };
    } else {
        GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
        if (GuiButton(Rectangle {static_cast<float>(screenWidth/2-100), screenHeight/2+25, 200, 50}, "Play Again") == 1) {
            gameInstance.gameState = gameInstance.gameOverReturnState;
            ResetGame(gameInstance);
        };
        if (GuiButton(Rectangle {static_cast<float>(screenWidth/2-100), screenHeight/2+85, 200, 50}, "Main Menu") == 1) {
            gameInstance.gameState = MAIN_MENU;
            ResetGame(gameInstance);
        };
    }
}

void DrawMainMenu(GameInstanceState &gameInstance) {
    DrawTextCentered("ScoreJam37", screenWidth/2-35, screenHeight/2-125, 50, BLACK);
    
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    if (GuiButton(Rectangle {static_cast<float>(screenWidth/2-100), screenHeight/2-50, 200, 50}, "Play") == 1) {
        gameInstance.gameState = PLAY;
        gameInstance.gameOverReturnState = PLAY;
    };
    if (GuiButton(Rectangle {static_cast<float>(screenWidth/2-100), screenHeight/2+10, 200, 50}, "Leaderboard") == 1) {
        gameInstance.gameState = LEADER_BOARD;
        selectLeaderboardMode(PLAY);
        selectedLeaderboardMode = PLAY;
        loadLeaderboardData(leaderboardNames, leaderboardScores);
    };
    if (GuiButton(Rectangle {static_cast<float>(screenWidth/2-100), screenHeight/2+70, 200, 50}, "Options") == 1) {
        gameInstance.gameState = OPTIONS;
    };
    // Hide exit button on web
    #if !defined(PLATFORM_WEB)
        if (GuiButton(Rectangle {static_cast<float>(screenWidth/2-100), screenHeight/2+130, 200, 50}, "Exit") == 1) {
            windowShouldClose = true;
        };
    #endif
}

void DrawOptions(GameInstanceState &gameInstance) {
    GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
                
    GuiGroupBox(Rectangle{37, 50, 640, 400}, "Options");
    
    GuiLabel(Rectangle {207, 170, 100, 25}, "SFX Volume");
    GuiSliderBar(Rectangle{302, 175, 120, 16}, NULL, (std::to_string((int)(gameSettings.sfxVolume*100)) + "%").c_str(), &gameSettings.sfxVolume, 0, 1);
    
    // SetSoundVolume(coneDrop, gameSettings.sfxVolume);
    
    if (GuiButton(Rectangle{282, 410, 150, 25}, "Back to Main Menu") == 1) {
        gameInstance.gameState = MAIN_MENU;
    }
}

void DrawLeaderboard(GameInstanceState &gameInstance) {
    GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    
    GuiGroupBox(Rectangle{37, 20, 640, 430}, "Leaderboard");
    
    selectedLeaderboardIndex = static_cast<int>(selectedLeaderboardMode);
    GuiToggleGroup(Rectangle{55, 35, 150, 25}, "Classic", &selectedLeaderboardIndex);
    if (static_cast<GameState>(selectedLeaderboardIndex) != selectedLeaderboardMode) {
        selectedLeaderboardMode = static_cast<GameState>(selectedLeaderboardIndex);
        selectLeaderboardMode(selectedLeaderboardMode);
        loadLeaderboardData(leaderboardNames, leaderboardScores);
    }
    
    GuiLabel(Rectangle{92, 65, 120, 25}, "Name");
    
    DrawLine(87, 65, 87, 400, BLACK);
    DrawLine(540, 65, 540, 400, BLACK);
    
    GuiLabel(Rectangle{545, 65, 120, 25}, "Score");
    
    DrawLine(62, 95, 640, 95, BLACK);
    
    float yValue = 100.0f;
    for (std::string name : leaderboardNames) {
        GuiLabel(Rectangle{92, yValue, 150, 25}, name.c_str());
        yValue += 30.0f;
    }
    yValue = 100.0f;
    for (int score : leaderboardScores) {
        GuiLabel(Rectangle{545, yValue, 150, 25}, std::to_string(score).c_str());
        yValue += 30.0f;
    }
    
    if (GuiButton(Rectangle{282, 410, 150, 25}, "Back to Main Menu") == 1) {
        gameInstance.gameState = MAIN_MENU;
    }
}

void UpdateGameInstance(GameInstanceState &gameInstance, RenderTexture2D &renderTexture, float relDt) {
    UpdateCamera(&gameInstance.camera, CAMERA_ORBITAL);
    
    // Draw
    BeginTextureMode(renderTexture);
        ClearBackground(Color{145, 255, 81, 255});
        DrawTexture(fieldTexture, 0, 0, Color{255,255,255,255});

        static float rotationAngle = 0.0f;
        GuiSliderBar(Rectangle{170, 10, 200, 20}, NULL, NULL, &rotationAngle, -180.0f, 180.0f);
        GuiLabel(Rectangle{10, 10, 250, 15}, ("Angle " + std::to_string(rotationAngle)).c_str());
        
        // 3D rendering
        BeginMode3D(gameInstance.camera);
            DrawModelEx(soccerBallModel, Vector3{0.0f, 0.0f, 0.0f}, 
                        Vector3{1.0f, 0.0f, 0.0f}, 
                        rotationAngle, Vector3{0.2f, 0.2f, 0.2f}, WHITE);
            DrawSphere(Vector3{0.0f, 0.0f, 0.0f}, 0.1f, RED);
        EndMode3D();
        
        // 2D rendering
        switch(gameInstance.gameState) {
            case GAME_OVER: {DrawGameOver(gameInstance); break;}
            case MAIN_MENU: {DrawMainMenu(gameInstance); break;}
            case OPTIONS: {DrawOptions(gameInstance); break;}
            case LEADER_BOARD: {DrawLeaderboard(gameInstance); break;}
            default: {break;}
        }
    EndTextureMode();
}

Texture2D LoadTextureFromImage2x(std::string filename) {
    Image image = LoadImage((assetPathPrefix + filename).c_str());
    ImageResizeNN(&image, image.width*2, image.height*2);
    return LoadTextureFromImage(image);
}

void init_app() {
    if (std::filesystem::exists("../assets") == false) {
        assetPathPrefix = "assets/";
    }
    
    InitAudioDevice();
    // coneFall = LoadSound((assetPathPrefix + "coneFall.ogg").c_str());

    mainRenderTexture = LoadRenderTexture(screenWidth, screenHeight);
    
    ResetGame(mainGameInstance);
    mainGameInstance.player = 1;

    fieldTexture = LoadTextureFromImage2x("field_layout.png");
    
    soccerBallModel = LoadModel((assetPathPrefix + "soccerBall/soccerBall.obj").c_str());
    
    selectLeaderboardMode(PLAY);
}

bool app_loop() {
    float relDt = GetFrameTime() * 60.0f; // Calculate delta time in relation to 60 frames per second
    
    BeginDrawing();
        ClearBackground(BLACK);
            UpdateGameInstance(mainGameInstance, mainRenderTexture, relDt);
            DrawTextureRec(mainRenderTexture.texture, Rectangle{0, 0, (float)mainRenderTexture.texture.width, (float)-mainRenderTexture.texture.height}, Vector2{0, 0}, WHITE);
    EndDrawing();
    
    return !windowShouldClose;
}

void deinit_app() {
    // UnloadSound(coneFall);
    CloseAudioDevice();
}