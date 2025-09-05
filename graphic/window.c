
#include "window.h"
#include "parking.h"

void init_window() {
    InitWindow(800, 600, "Parking Simulator");
    SetTargetFPS(60);

    background = LoadTexture("Assets/background.png");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(background, 0, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(background);
    CloseWindow();
}