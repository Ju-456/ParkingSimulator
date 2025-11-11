#include "window.h"

void load_textures() {
    background = LoadTexture("Assets/background.png");
    free_parking_place = LoadTexture("Assets/free_parking_place.png");
    busy_parking_place = LoadTexture("Assets/busy_parking_place.png");
    panel_menu = LoadTexture("Assets/panel_menu.png");

    entrance_barrier = LoadTexture("Assets/entrance_barrier.png");
    entrance_ticket_dispenser = LoadTexture("Assets/entrance_ticket_dispenser.png");

    exit_barrier = LoadTexture("Assets/exit_barrier.png");
    exit_pay_station = LoadTexture("Assets/exit_pay_station.png");

    barrier_wall = LoadTexture("Assets/barrier_wall.png");
    car_preview = LoadTexture("Assets/car_preview.png");

    floor_exit = LoadTexture("Assets/floor_exit.png");
    floor_indicator[0] = LoadTexture("Assets/floor_indicator0.png");
    floor_indicator[1] = LoadTexture("Assets/floor_indicator1.png");
    floor_indicator[2] = LoadTexture("Assets/floor_indicator2.png");

    PC = LoadTexture("Assets/PC.png");

    const char *colorNames[NUM_CARS] = {"black", "blue", "gray", "pink", "red", "yellow"};

    for (int i = 0; i < NUM_CARS; i++) {
        char path[128];

        // Right texture
        snprintf(path, sizeof(path), "Assets/%s_car/%sRight.png", colorNames[i], colorNames[i]);
        carOrient[i].right = LoadTexture(path);

        // Front texture
        snprintf(path, sizeof(path), "Assets/%s_car/%sFront.png", colorNames[i], colorNames[i]);
        carOrient[i].front = LoadTexture(path);

        // Top texture
        snprintf(path, sizeof(path), "Assets/%s_car/%sTop.png", colorNames[i], colorNames[i]);
        carOrient[i].top = LoadTexture(path);
    }
}

void unload_textures() {
    UnloadTexture(background);
    UnloadTexture(free_parking_place);
    UnloadTexture(busy_parking_place);
    UnloadTexture(panel_menu);

    UnloadTexture(entrance_barrier);
    UnloadTexture(entrance_ticket_dispenser);

    UnloadTexture(exit_barrier);
    UnloadTexture(exit_pay_station);

    UnloadTexture(barrier_wall);

    UnloadTexture(floor_exit);
    UnloadTexture(floor_indicator[0]);
    UnloadTexture(floor_indicator[1]);
    UnloadTexture(floor_indicator[2]);
    UnloadTexture(car_preview);

    UnloadTexture(PC);

    for (int i = 0; i < NUM_CARS; i++) {
        UnloadTexture(carOrient[i].front);
        UnloadTexture(carOrient[i].right);
        UnloadTexture(carOrient[i].top);
    }
}