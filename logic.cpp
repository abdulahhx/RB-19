include <iostream>
#include <random>
#include <fstream>
#include "raylib.h"

using namespace std;

// Game Details
enum GameScreen { MENU, GAMEPLAY, GAMEOVER, YOUWIN };

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Space Shooter");
    SetTargetFPS(60);

    InitAudioDevice();

    Sound shootSound = LoadSound("shoot.wav");  
    Sound explosionSound = LoadSound("explosion.wav");
    Sound hitSound = LoadSound("hit.wav"); 
    Sound levelUpSound = LoadSound("levelup.wav");
    Texture2D playerTexture = LoadTexture("spaceship.png");
    Texture2D enemyTexture = LoadTexture("enemy.png");
    Texture2D backgroundTexture = LoadTexture("background.png");

    GameScreen currentScreen = MENU;

    int level = 1;
    int selectedLevel = 1;
    int lives = 3;
    int score = 0;
    int highscore = 0;

    // PLAYER
    int playerX = screenWidth / 2;
    int playerY = screenHeight - 80;
    int playerSpeed = 5;
    int basePlayerSpeed = 5;

    // BULLET
    int maxBulletCount = 10;
    int bulletCount = 0;
    bool bulletActive[10];
    int bulletX[10];
    int bulletY[10];
    int bulletSpeed = 7;

    for (int i = 0; i < maxBulletCount; i++) {
        bulletActive[i] = false;
    }

    // ENEMIES
    int maxEnemyCount = 10;
    int enemyCount = 0;
    int enemyX[10];
    int enemyY[10];
    int enemyFlag[10];
    int enemySpeed = 1;
    int baseEnemySpeed = 1;

    int enemySpawnRate = 50;
    // Initialize enemies
    for (int i = 0; i < maxEnemyCount; i++) {
        enemyX[i] = rand() % (screenWidth - 80) + 40;
        enemyY[i] = -40;
        enemyFlag[i] = 0;
    }
    int enemyStep = 0;

    // Load highscore from file
    ifstream highscoreFileIn("highscore.txt");
    if (highscoreFileIn.is_open()) {
        highscoreFileIn >> highscore;
        highscoreFileIn.close();
    }
    else {
        highscore = 0;
    }

    while (!WindowShouldClose()) {

        // ESC key to return to menu
        if (IsKeyPressed(KEY_ESCAPE) && currentScreen != MENU) {
            currentScreen = MENU;
            // Reset game
            lives = 3;
            level = selectedLevel;
            score = 0;
            enemySpeed = baseEnemySpeed + (selectedLevel - 1);
            enemyCount = 0;
            playerSpeed = basePlayerSpeed + (selectedLevel - 1);
            playerX = screenWidth / 2;
            playerY = screenHeight - 80;
            bulletCount = 0;
            for (int i = 0; i < maxBulletCount; i++) {
                bulletActive[i] = false;
            }
            for (int i = 0; i < maxEnemyCount; i++) {
                enemyFlag[i] = 0;
            }
        }

        //  MENU SCREEN 
        if (currentScreen == MENU) {
            // Level selection
            if (IsKeyPressed(KEY_ONE)) selectedLevel = 1;
            if (IsKeyPressed(KEY_TWO)) selectedLevel = 2;
            if (IsKeyPressed(KEY_THREE)) selectedLevel = 3;
            if (IsKeyPressed(KEY_FOUR)) selectedLevel = 4;
            if (IsKeyPressed(KEY_FIVE)) selectedLevel = 5;

            // Start game
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = GAMEPLAY;
                level = selectedLevel;
                lives = 3;
                score = 0;
                enemySpeed = baseEnemySpeed + (selectedLevel - 1);
                playerSpeed = basePlayerSpeed + (selectedLevel - 1);
                enemyCount = 0;
                playerX = screenWidth / 2;
                playerY = screenHeight - 80;
                bulletCount = 0;
                for (int i = 0; i < maxBulletCount; i++) {
                    bulletActive[i] = false;
                }
                for (int i = 0; i < maxEnemyCount; i++) {
                    enemyFlag[i] = 0;
                }
            }

            // Code for Menu
            BeginDrawing();
            DrawTexture(backgroundTexture, 0, 0, WHITE);

            DrawText("SPACE SHOOTER", screenWidth / 2 - 200, 80, 50, SKYBLUE);
            DrawText("INSTRUCTIONS:", 100, 200, 25, WHITE);
            DrawText("- Use ARROW KEYS or A/D to move", 120, 240, 20, LIGHTGRAY);
            DrawText("- Press SPACE to shoot", 120, 270, 20, LIGHTGRAY);
            DrawText("- Destroy enemies to score points", 120, 300, 20, LIGHTGRAY);
            DrawText("- Reach 10 points per level to advance", 120, 330, 20, LIGHTGRAY);
            DrawText("- Press ESC anytime to return to menu", 120, 360, 20, LIGHTGRAY);

            DrawText("SELECT LEVEL (Press 1-5):", 100, 420, 25, YELLOW);
            DrawText(TextFormat("Current Selection: Level %d", selectedLevel), 100, 460, 22, WHITE);

            DrawText("Press ENTER to Start", screenWidth / 2 - 140, 520, 25, RED);
            DrawText(TextFormat("High Score: %d", highscore), 10, 10, 20, GOLD);

            EndDrawing();
        }

        // Game Play
        else if (currentScreen == GAMEPLAY) {

            //Enemy Logic
            if (++enemyStep == enemySpawnRate && enemyCount < maxEnemyCount) {
                enemyFlag[enemyCount] = 1;
                enemyX[enemyCount] = rand() % (screenWidth - 80) + 40;
                enemyY[enemyCount] = -40;
                enemyCount++;
                enemyStep = 0;
                enemySpawnRate = (rand() % 100) + 50;
            }

            for (int i = 0; i < enemyCount; i++) {
                enemyY[i] += enemySpeed;
                if (enemyY[i] > playerY - 40) {
                    lives--;
                    PlaySound(hitSound);

                    enemyCount--;
                    enemyX[i] = enemyX[enemyCount];
                    enemyY[i] = enemyY[enemyCount];
                    enemyFlag[enemyCount] = 0;

                    break;
                }
            }

            // New Level
            if (score == level * 10) {
                level++;
                PlaySound(levelUpSound);

                enemySpeed++;
                enemyCount = 0;
                for (int i = 0; i < maxEnemyCount; i++) {
                    enemyX[i] = rand() % (screenWidth - 80) + 40;
                    enemyY[i] = -40;
                    enemyFlag[i] = 0;
                }
                playerSpeed++;
                playerX = screenWidth / 2;
                playerY = screenHeight - 80;

                bulletCount = 0;
                for (int i = 0; i < maxBulletCount; i++) {
                    bulletActive[i] = false;
                }
            }

            // Death
            if (lives <= 0) {
                currentScreen = GAMEOVER;

                // Update highscore
                if (score > highscore) {
                    highscore = score;
                    ofstream highscoreFileOut("highscore.txt");
                    if (highscoreFileOut.is_open()) {
                        highscoreFileOut << highscore;
                        highscoreFileOut.close();
                    }
                }
            }

            // Win
            if (level == 6) {
                currentScreen = YOUWIN;

                // Update highscore
                if (score > highscore) {
                    highscore = score;
                    ofstream highscoreFileOut("highscore.txt");
                    if (highscoreFileOut.is_open()) {
                        highscoreFileOut << highscore;
                        highscoreFileOut.close();
                    }
                }
            }

            // Player Movement
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerX -= playerSpeed;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerX += playerSpeed;

            if (playerX < 0) playerX = 0;
            if (playerX > screenWidth - 40) playerX = screenWidth - 40;

            //Bullet Logic
            if (IsKeyPressed(KEY_SPACE) && bulletCount < maxBulletCount) {
                bulletActive[bulletCount] = true;
                bulletX[bulletCount] = playerX + 18;
                bulletY[bulletCount] = playerY;
                bulletCount++;
                PlaySound(shootSound);
            }

            for (int i = 0; i < bulletCount; i++) {
                if (bulletActive[i]) {
                    bulletY[i] -= bulletSpeed;

                    if (bulletY[i] < 0) {
                        bulletCount--;
                        bulletX[i] = bulletX[bulletCount];
                        bulletY[i] = bulletY[bulletCount];
                        bulletActive[bulletCount] = false;
                    }
                }

                // Bullet and Enemy Collision Logic
                if (bulletActive[i]) {
                    for (int j = 0; j < enemyCount; j++) {
                        if (bulletX[i] + 10 > enemyX[j] && bulletX[i] < enemyX[j] + 40 &&
                            bulletY[i] + 10 > enemyY[j] && bulletY[i] < enemyY[j] + 40) {

                            // enemy "dies"
                            PlaySound(explosionSound);
                            enemyCount--;
                            enemyX[j] = enemyX[enemyCount];
                            enemyY[j] = enemyY[enemyCount];
                            enemyFlag[enemyCount] = 0;

                            bulletCount--;
                            bulletX[i] = bulletX[bulletCount];
                            bulletY[i] = bulletY[bulletCount];
                            bulletActive[bulletCount] = false;
                            score++;

                            break;
                        }
                    }
                }
            }

            // Draw the screen
            BeginDrawing();
            DrawTexture(backgroundTexture, 0, 0, WHITE);

            DrawText(TextFormat("Level: %d", level), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, WHITE);
            DrawText(TextFormat("Score: %d", score), 10, 70, 20, WHITE);
            DrawText(TextFormat("High Score: %d", highscore), 10, 100, 20, GOLD);

            for (int i = 0; i < enemyCount; i++) {
                DrawTexture(enemyTexture, enemyX[i], enemyY[i], WHITE);
            }

            for (int i = 0; i < bulletCount; i++) {
                if (bulletActive[i]) {
                    DrawRectangle(bulletX[i], bulletY[i], 5, 10, YELLOW);
                }
            }
            DrawTexture(playerTexture, playerX, playerY, WHITE);

            EndDrawing();
        }

        //  Game Over
        else if (currentScreen == GAMEOVER) {
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = MENU;
            }

            BeginDrawing();
            DrawTexture(backgroundTexture, 0, 0, WHITE);

            DrawText("GAME OVER", screenWidth / 2 - 150, 200, 50, RED);
            DrawText(TextFormat("Final Score: %d", score), screenWidth / 2 - 120, 300, 30, WHITE);
            DrawText(TextFormat("High Score: %d", highscore), screenWidth / 2 - 120, 350, 30, GOLD);
            DrawText("Press ENTER to return to Menu", screenWidth / 2 - 200, 450, 25, LIGHTGRAY);
            DrawText("Press ESC to return to Menu", screenWidth / 2 - 180, 490, 20, GRAY);

            EndDrawing();
        }

        //  Win Screen
        else if (currentScreen == YOUWIN) {
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = MENU;
            }

            BeginDrawing();
            DrawTexture(backgroundTexture, 0, 0, WHITE);

            DrawText("YOU WIN!", screenWidth / 2 - 130, 200, 50, GREEN);
            DrawText("CONGRATULATIONS!", screenWidth / 2 - 180, 270, 35, SKYBLUE);
            DrawText(TextFormat("Final Score: %d", score), screenWidth / 2 - 120, 340, 30, WHITE);
            DrawText(TextFormat("High Score: %d", highscore), screenWidth / 2 - 120, 390, 30, GOLD);
            DrawText("Press ENTER to return to Menu", screenWidth / 2 - 200, 480, 25, LIGHTGRAY);
            DrawText("Press ESC to return to Menu", screenWidth / 2 - 180, 520, 20, GRAY);

            EndDrawing();
        }
    }

    // Unloading the sounds
    UnloadSound(shootSound);
    UnloadSound(explosionSound);
    UnloadSound(hitSound);
    UnloadSound(levelUpSound);
    UnloadTexture(playerTexture);
    UnloadTexture(enemyTexture);
    UnloadTexture(backgroundTexture);
    CloseAudioDevice();

    CloseWindow();
    return 0;

}
