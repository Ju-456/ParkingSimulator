#include "window.h"

void full_screen_mode(int num_parking_places, Parking places[], Parking scaled_places[]) {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    float scale = fminf((float)screen_width / SCREEN_WIDTH, (float)screen_height / SCREEN_HEIGHT);
    float offsetX = (screen_width - SCREEN_WIDTH * scale) / 2.0f;
    float offsetY = (screen_height - SCREEN_HEIGHT * scale) / 2.0f;

    for (int i = 0; i < num_parking_places; i++) {
        scaled_places[i] = places[i];
        scaled_places[i].x = places[i].x * scale + offsetX;
        scaled_places[i].y = places[i].y * scale + offsetY;
    }
}

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

void init_window(const char *full_path_json, int num_parking_places, Parking places[], Parking scaled_places[]) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Parking Simulator");
    SetTargetFPS(60);

    background1 = LoadTexture("Assets/background1.png");
    parkingPlace = LoadTexture("Assets/parkingPlace.png");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(background1, 0, 0, WHITE);
        full_screen_mode(num_parking_places, places, scaled_places);
        draw_parking_places(num_parking_places, scaled_places);

        EndDrawing();
    }

    UnloadTexture(background1);
    CloseWindow();
}