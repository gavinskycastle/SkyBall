#include "../libs/raylib/src/raylib.h"
#include "helper.hpp"

#define RAYGUI_IMPLEMENTATION
#include "../libs/raygui/src/raygui.h"

#include <filesystem>

#include "main.hpp"
#include "leaderboard.hpp"
#include <raymath.h>

// Window setup
int screenWidth = 840; // Sprites are 2x scaled, so this will be 420x280
const int screenHeight = 560;
bool windowShouldClose = false;

// Loading resources
std::string assetPathPrefix = "../assets/";

Texture2D fieldTexture;
Texture2D goalTexture;
Texture2D backgroundTexture;

Color grassGreen = Color{89, 175, 35, 255};
Rectangle fieldBounds = Rectangle{54, 34, 732, 492};
Rectangle goalBoundA = Rectangle{0, 178, 64, 200};
Rectangle goalBoundB = Rectangle{776, 178, 64, 200};

// Leaderboard/name entry setup
bool highScoreEditMode = false;
char highScoreName[17] = "";
std::vector<std::string> leaderboardNames;
std::vector<int> leaderboardScores;
GameState selectedLeaderboardMode;
int selectedLeaderboardIndex = 0;

// Global game state setup
GameSettings gameSettings;

GameInstanceState mainGameInstance;

void DrawTextCentered(const char* text, int posX, int posY, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, posX-(textWidth/2), posY, fontSize, color);
}

void ResetGame(GameInstanceState &gameInstance) {
    gameInstance.score = 0;
    gameInstance.round = 1;
    gameInstance.player = Player();
    gameInstance.ball = new Ball(screenWidth / 2, screenHeight / 2);
    gameInstance.ball->init(assetPathPrefix);
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
    DrawTextCentered("SkyBall", screenWidth/2, screenHeight/2-125, 50, BLACK);
    
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
    
    //selectedLeaderboardIndex = static_cast<int>(selectedLeaderboardMode);
    //GuiToggleGroup(Rectangle{55, 35, 150, 25}, "Classic", &selectedLeaderboardIndex);
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

void IterateToNextRound(GameInstanceState &gameInstance) {
    if (gameInstance.round == 10) {
        gameInstance.gameState = GAME_OVER;
        gameInstance.gameOverReturnState = PLAY;
    } else {
        gameInstance.round += 1;
        // Reset player and ball
        gameInstance.player = Player();
        delete gameInstance.ball;
        gameInstance.ball = new Ball(screenWidth / 2, screenHeight / 2);
        gameInstance.ball->init(assetPathPrefix);
    }
}

void UpdateGameInstance(GameInstanceState &gameInstance, float relDt) {
    ClearBackground(BLANK);
    DrawTexture(backgroundTexture, 0, 0, WHITE);
    DrawRectangleRec(fieldBounds, grassGreen);
    DrawRectangleRec(goalBoundA, grassGreen);
    DrawRectangleRec(goalBoundB, grassGreen);
    DrawTexture(fieldTexture, 0, 0, WHITE);
    
    //Player Moving (Walking and Running)
    gameInstance.player.playerMovement(relDt, fieldBounds);

    // Only 3D rendered object
    std::vector<Player> players = {gameInstance.player};
    bool stateChanged = gameInstance.ball->update(relDt, players, fieldBounds, goalBoundA, goalBoundB);
    if (stateChanged) {
        if (gameInstance.ball->ballState == SCORED) {
            gameInstance.score = static_cast<int>(50.0f + pow(1.17, ((gameInstance.ball->velocityMultiplier - 0.5f) * 50 + 5))); //score is y=50+1.17^(50x+5) where x is number of kicks
            IterateToNextRound(gameInstance);
        } else if (gameInstance.ball->ballState == FALLEN) {
            IterateToNextRound(gameInstance);
        }
    }
    // // Kick ball with arrow keys
    // if (IsKeyPressed(KEY_UP)) {
    //     gameInstance.ball->kick(0.0f, -1.0f);
    // }
    // if (IsKeyPressed(KEY_DOWN)) {
    //     gameInstance.ball->kick(0.0f, 1.0f);
    // }
    // if (IsKeyPressed(KEY_LEFT)) {
    //     gameInstance.ball->kick(-1.0f, 0.0f);
    // }
    // if (IsKeyPressed(KEY_RIGHT)) {
    //     gameInstance.ball->kick(1.0f, 0.0f);
    // }
    
    DrawRectangleLines(screenWidth/2-100, 0, 200, 60, BLACK);
    DrawRectangleLines(screenWidth/2-100, 59, 200, 20, BLACK);
    DrawTextCentered(std::to_string(gameInstance.score).c_str(), screenWidth/2, 10, 50, BLACK);
    DrawTextCentered(("Round " + std::to_string(gameInstance.round)).c_str(), screenWidth/2, 62, 15, BLACK);
    
    DrawTexture(goalTexture, 4, 182, WHITE);
    DrawTexture(goalTexture, 772, 182, WHITE);
    
    if (IsKeyPressed(KEY_L)) {
        gameInstance.gameState = GAME_OVER;
    }
    
    
    // 2D rendering
    switch(gameInstance.gameState) {
        case GAME_OVER: {DrawGameOver(gameInstance); break;}
        case MAIN_MENU: {DrawMainMenu(gameInstance); break;}
        case OPTIONS: {DrawOptions(gameInstance); break;}
        case LEADER_BOARD: {DrawLeaderboard(gameInstance); break;}
        default: {break;}
    }
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
    
    ResetGame(mainGameInstance);

    fieldTexture = LoadTextureFromImage2x("field_layout.png");
    goalTexture = LoadTextureFromImage2x("goal.png");
    backgroundTexture = LoadTextureFromImage2x("background.png");
    
    selectLeaderboardMode(PLAY);
}

bool app_loop() {
    float relDt = GetFrameTime() * 60.0f; // Calculate delta time in relation to 60 frames per second
    
    BeginDrawing();
        UpdateGameInstance(mainGameInstance, relDt);
    EndDrawing();
    
    return !windowShouldClose;
}

void deinit_app() {
    // UnloadSound(coneFall);
    CloseAudioDevice();
}