#include "window.h"

void draw_parking_places(int n, Parking places[]) {
    const float width = 180.0f;
    const float height = 95.0f;
    const float scale = 0.6f; // 60% of the real size

    for (int i = 0; i < n; i++) {
        float w = width * scale;
        float h = height * scale;

        Rectangle src = (Rectangle){ 0, 0, width, height };
        Rectangle dest = (Rectangle){ places[i].x, places[i].y, w, h };

        if (places[i].direction == 0) { // positive direction
            DrawTexturePro(parkingPlace, src, dest, (Vector2){0,0}, 0.0f, WHITE);
        } else { // negative direction => Renversed picture left/right
            Rectangle srcMir = (Rectangle){ width, 0, -width, height };
            DrawTexturePro(parkingPlace, srcMir, dest, (Vector2){0,0}, 0.0f, WHITE);
        }
    }
}


void init_window(const char *full_path_json, int num_parking_places, Parking places[]) {
    InitWindow(800, 600, "Parking Simulator");
    SetTargetFPS(60);

    background = LoadTexture("Assets/background.png");
    parkingPlace = LoadTexture("Assets/parkingPlace.png");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(background, 0, 0, WHITE);
        draw_parking_places(num_parking_places, places);

        EndDrawing();
    }

    UnloadTexture(background);
    CloseWindow();
}