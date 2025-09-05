#include "window.h"

void init_window(const char *full_path_json, int num_parking_places, Parking places[]) {
    InitWindow(800, 600, "Parking Simulator");
    SetTargetFPS(60);

    background = LoadTexture("Assets/background.png");
    load_graph_from_json(full_path_json, num_parking_places, places);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(background, 0, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(background);
    CloseWindow();
}