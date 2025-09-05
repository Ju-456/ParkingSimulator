#include "window.h"

// void draw_parking_places(int num_parking_places){
//     for (int i = 0; i < num_parking_places; i++){// Horizontal
//         Rectangle src = {180, 95, 100, 95}; 
//                 Rectangle dest = {
//                     minX, y - 16,
//                     maxX - minX, 32
//                 };

//         DrawTexturePro(road_texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
//     }
// }

void init_window(const char *full_path_json, int num_parking_places, Parking places[]) {
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