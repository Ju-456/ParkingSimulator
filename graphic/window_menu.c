#include "window_menu.h"

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
            DrawTexturePro(parking_place, src, dest, (Vector2){0,0}, 0.0f, WHITE);
        } else { // negative direction => Renversed picture left/right
            Rectangle srcMir = (Rectangle){ width, 0, -width, height };
            DrawTexturePro(parking_place, srcMir, dest, (Vector2){0,0}, 0.0f, WHITE);
        }
    }
}

void pannel(){
    DrawTexture(pannel_menu, 0, 560, WHITE);
}

void draw_entrance_barrier(){
    const float entrance_barrier_width = 94.0f;
    const float entrance_barrier_height = 18.0f;
    const float barrier_wall_width = 134.0f;
    const float barrier_wall_height = 5.0f;

    DrawTexture(entrance_barrier, 140, 10, WHITE);
    DrawTexture(barrier_wall, 10, entrance_barrier_width + 15, WHITE);

}
void draw_exit_barrier(){
    const float exit_barrier_width = 94.0f;
    const float exit_barrier_heightç = 18.0f;
    const float barrier_wall_width = 134.0f;
    const float barrier_wall_height = 5.0f;

    DrawTexture(exit_barrier, 645, 456, WHITE);
    DrawTexture(barrier_wall, 660, 450, WHITE);
    
}

void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[], Parking scaled_places[]) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Parking Simulator");
    SetTargetFPS(60);

    background = LoadTexture("Assets/background.png");
    parking_place = LoadTexture("Assets/parking_place.png");
    pannel_menu = LoadTexture("Assets/pannel_menu.png");
    entrance_barrier = LoadTexture("Assets/entrance_barrier.png");
    exit_barrier = LoadTexture("Assets/exit_barrier.png");
    barrier_wall = LoadTexture("Assets/barrier_wall.png");
    //entrance_pay_station = LoadTexture("Assets/entrance_pay_station.png");
    //exit_ticket_dispenser = LoadTexture("Assets/exit_ticket_dispenser.png");



    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);

        pannel();
        draw_entrance_barrier();
        draw_exit_barrier();
        full_screen_mode(num_parking_places, places, scaled_places);
        draw_parking_places(num_parking_places, scaled_places);

        EndDrawing();
    }

    UnloadTexture(background);
    UnloadTexture(parking_place);
    UnloadTexture(background);
    UnloadTexture(entrance_barrier);
    UnloadTexture(exit_barrier);
    CloseWindow();
}