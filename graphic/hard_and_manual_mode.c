#include "window.h"

// AI Cars for hard mode
#define NUM_AI_CARS 3
#define MAX_AVAILABLE_SIMS 5
static AICar aiCars[NUM_AI_CARS];
static FILE *aiSimFiles[NUM_AI_CARS] = {NULL, NULL, NULL};
static int aiSimActive[NUM_AI_CARS] = {0, 0, 0};
static int aiCarColors[NUM_AI_CARS] = {CAR_BLUE, CAR_RED, CAR_YELLOW};
static double aiSimTime[NUM_AI_CARS] = {0.0, 0.0, 0.0};
static int selectedSimulations[NUM_AI_CARS] = {0, 0, 0};

void init_hard_mode_ai_cars() {
    int numSims = (rand() % 2) + 2;

    int availableSims[MAX_AVAILABLE_SIMS];
    for (int i = 0; i < MAX_AVAILABLE_SIMS; i++) {
        availableSims[i] = i;
    }

    for (int i = MAX_AVAILABLE_SIMS - 1; i > MAX_AVAILABLE_SIMS - numSims - 1; i--) {
        int j = rand() % (i + 1);
        int temp = availableSims[i];
        availableSims[i] = availableSims[j];
        availableSims[j] = temp;
    }

    // Assign selected simulations to AI cars
    for (int i = 0; i < NUM_AI_CARS; i++) {
        aiCars[i].x = 0.0f;
        aiCars[i].y = 0.0f;
        aiCars[i].rotation = 0.0f;
        aiCars[i].floor = 0;
        aiCars[i].colorIndex = aiCarColors[i];
        aiCars[i].active = 1;

        selectedSimulations[i] = (i < numSims) ? availableSims[i] : 0;

        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "graphic/simdata/hard_mode/simulation_data_%d.txt", selectedSimulations[i]);
        aiSimFiles[i] = fopen(filepath, "r");

        if (aiSimFiles[i]) {
            aiSimActive[i] = 1;
            printf("AI car %d using simulation %d (%s)\n", i, selectedSimulations[i], filepath);
        } else {
            printf("Failed to open AI car %d simulation: %s\n", i, filepath);
            aiSimActive[i] = 0;
        }

        aiSimTime[i] = 0.0;
    }
}

// Update AI cars based on simulation files
void update_ai_simulation(float dt, int carIndex) {
    if (carIndex < 0 || carIndex >= NUM_AI_CARS || !aiSimActive[carIndex] || !aiSimFiles[carIndex]) {
        return;
    }

    aiSimTime[carIndex] += dt;

    char line[512];
    if (fgets(line, sizeof(line), aiSimFiles[carIndex])) {
        float rx, ry, rrot;
        int rfloor;
        double rtime;

        int scanned = sscanf(line, "Car X: %f, Car Y: %f, Rotation: %f, Floor: %d, simTime : %lf", &rx, &ry, &rrot, &rfloor, &rtime);

        if (scanned < 4) {
            scanned = sscanf(line, "Car X:%f ,Car Y:%f ,Rotation:%f ,Floor:%d ,simTime :%lf", &rx, &ry, &rrot, &rfloor, &rtime);
        }

        if (scanned >= 4) {
            aiCars[carIndex].x = rx;
            aiCars[carIndex].y = ry;
            aiCars[carIndex].rotation = rrot;
            aiCars[carIndex].floor = rfloor;
        }
    } else {
        // End of file reached, rewind or deactivate
        rewind(aiSimFiles[carIndex]);
        aiSimActive[carIndex] = 0;
    }
}

// Update all AI cars
void update_hard_mode_ai_cars(float dt) {
    for (int i = 0; i < NUM_AI_CARS; i++) {
        if (aiCars[i].active) {
            update_ai_simulation(dt, i);
        }
    }
}

void draw_hard_mode_ai_cars() {
    for (int i = 0; i < NUM_AI_CARS; i++) {
        if (!aiCars[i].active || aiCars[i].floor != carFloor) {
            continue;
        }

        float scale = 0.65f;
        int colorIdx = aiCars[i].colorIndex;

        switch (colorIdx) {
        case CAR_BLACK:
        case CAR_GRAY:
            scale = 0.65f;
            break;
        case CAR_BLUE:
            scale = 0.65f;
            break;
        case CAR_PINK:
            scale = 0.6f;
            break;
        case CAR_RED:
            scale = 0.9f;
            break;
        case CAR_YELLOW:
            scale = 0.65f;
            break;
        default:
            scale = 0.65f;
        }

        DrawTexturePro(carOrient[colorIdx].top, (Rectangle){0, 0, carOrient[colorIdx].top.width, carOrient[colorIdx].top.height},
                       (Rectangle){aiCars[i].x, aiCars[i].y, carOrient[colorIdx].top.width * scale, carOrient[colorIdx].top.height * scale},
                       (Vector2){carOrient[colorIdx].top.width * scale / 2, carOrient[colorIdx].top.height * scale / 2}, aiCars[i].rotation, WHITE);
    }
}

void cleanup_hard_mode_ai_cars() {
    for (int i = 0; i < NUM_AI_CARS; i++) {
        if (aiSimFiles[i]) {
            fclose(aiSimFiles[i]);
            aiSimFiles[i] = NULL;
        }
        aiSimActive[i] = 0;
    }
}

void choose_your_car(Font font) {
    const char *message = "Choose a car :";

    static int letters = 0;
    static float timer = 0.0f;

    float letterDelay = 0.07f;
    float dt = GetFrameTime();

    if (letters < strlen(message)) {
        timer += dt;
        if (timer >= letterDelay) {
            letters++;
            timer = 0.0f;
        }
    }

    DrawTextEx(font, TextSubtext(message, 0, letters), (Vector2){300, 590}, 24, 2, WHITE);

    float scale = 0.8f;

    // black car
    DrawTextureRec(PC, srcMode, (Vector2){btnRandom.x, btnRandom.y - 70}, WHITE);
    DrawTexturePro(carOrient[CAR_BLACK].right, (Rectangle){0, 0, carOrient[CAR_BLACK].right.width, carOrient[CAR_BLACK].right.height},
                   (Rectangle){btnRandom.x + 10, btnRandom.y - 65, srcMode.width * scale, srcMode.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);

    // blue car
    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y - 70}, WHITE);
    DrawTexturePro(carOrient[CAR_BLUE].right, (Rectangle){0, 0, carOrient[CAR_BLUE].right.width, carOrient[CAR_BLUE].right.height},
                   (Rectangle){btnManual.x + 10, btnManual.y - 65, srcMode.width * scale, srcMode.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);

    // gray car
    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y - 70}, WHITE);
    DrawTexturePro(carOrient[CAR_GRAY].right, (Rectangle){0, 0, carOrient[CAR_GRAY].right.width, carOrient[CAR_GRAY].right.height},
                   (Rectangle){btnHardManual.x + 10, btnHardManual.y - 65, srcMode.width * scale, srcMode.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);

    // pink car
    DrawTextureRec(PC, srcMode, (Vector2){btnRandom.x, btnRandom.y}, WHITE);
    DrawTexturePro(carOrient[CAR_PINK].right, (Rectangle){0, 0, carOrient[CAR_PINK].right.width, carOrient[CAR_PINK].right.height},
                   (Rectangle){btnRandom.x + 10, btnManual.y, srcMode.width * scale, srcMode.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);

    // red car
    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y}, WHITE);
    DrawTexturePro(carOrient[CAR_RED].right, (Rectangle){0, 0, carOrient[CAR_RED].right.width, carOrient[CAR_RED].right.height},
                   (Rectangle){btnManual.x + 10, btnManual.y, srcMode.width * scale, srcMode.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);

    // yellow car
    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y}, WHITE);
    DrawTexturePro(carOrient[CAR_YELLOW].right, (Rectangle){0, 0, carOrient[CAR_YELLOW].right.width, carOrient[CAR_YELLOW].right.height},
                   (Rectangle){btnHardManual.x + 10, btnHardManual.y, srcMode.width * scale, srcMode.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);

    choose_your_car_condition();
}

void choose_your_car_condition() {
    float scale = 1.f;

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, (Rectangle){btnRandom.x, btnRandom.y - 70, srcMode.width, srcMode.height})) {
            chosenCar = 0; // black car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnManual.x, btnManual.y - 70, srcMode.width, srcMode.height})) {
            chosenCar = 1; // blue car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnHardManual.x, btnHardManual.y - 70, srcMode.width, srcMode.height})) {
            chosenCar = 2; // gray car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnRandom.x, btnRandom.y, srcMode.width, srcMode.height})) {
            chosenCar = 3; // pink car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnManual.x, btnManual.y, srcMode.width, srcMode.height})) {
            chosenCar = 4; // red car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnHardManual.x, btnHardManual.y, srcMode.width, srcMode.height})) {
            chosenCar = 5; // yellow car
        }
    }

    if (chosenCar == 0) {
        DrawTextureRec(car_preview, srcPreview, (Vector2){20, 580}, WHITE);
        DrawTexturePro(carOrient[CAR_BLACK].front, (Rectangle){0, 0, carOrient[CAR_BLACK].front.width, carOrient[CAR_BLACK].front.height},
                       (Rectangle){50, 590, carOrient[CAR_BLACK].front.width * scale, carOrient[CAR_BLACK].front.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);
    } else if (chosenCar == 1) {
        DrawTextureRec(car_preview, srcPreview, (Vector2){20, 580}, WHITE);
        DrawTexturePro(carOrient[CAR_BLUE].front, (Rectangle){0, 0, carOrient[CAR_BLUE].front.width, carOrient[CAR_BLUE].front.height},
                       (Rectangle){50, 590, carOrient[CAR_BLUE].front.width * scale, carOrient[CAR_BLUE].front.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);
    } else if (chosenCar == 2) {
        DrawTextureRec(car_preview, srcPreview, (Vector2){20, 580}, WHITE);
        DrawTexturePro(carOrient[CAR_GRAY].front, (Rectangle){0, 0, carOrient[CAR_GRAY].front.width, carOrient[CAR_GRAY].front.height},
                       (Rectangle){50, 590, carOrient[CAR_GRAY].front.width * scale, carOrient[CAR_GRAY].front.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);
    } else if (chosenCar == 3) {
        DrawTextureRec(car_preview, srcPreview, (Vector2){20, 580}, WHITE);
        DrawTexturePro(carOrient[CAR_PINK].front, (Rectangle){0, 0, carOrient[CAR_PINK].front.width, carOrient[CAR_PINK].front.height},
                       (Rectangle){50, 590, carOrient[CAR_PINK].front.width * scale, carOrient[CAR_PINK].front.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);
    } else if (chosenCar == 4) {
        DrawTextureRec(car_preview, srcPreview, (Vector2){20, 580}, WHITE);
        DrawTexturePro(carOrient[CAR_RED].front, (Rectangle){0, 0, carOrient[CAR_RED].front.width, carOrient[CAR_RED].front.height},
                       (Rectangle){55, 590, carOrient[CAR_RED].front.width * scale, carOrient[CAR_RED].front.height * scale}, (Vector2){0, 0}, 0.0f, WHITE);
    } else if (chosenCar == 5) {
        DrawTextureRec(car_preview, srcPreview, (Vector2){20, 580}, WHITE);
        DrawTexturePro(carOrient[CAR_YELLOW].front, (Rectangle){0, 0, carOrient[CAR_YELLOW].front.width, carOrient[CAR_YELLOW].front.height},
                       (Rectangle){55, 590, carOrient[CAR_YELLOW].front.width * 0.9, carOrient[CAR_YELLOW].front.height * 0.9}, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

void update_car_position(float dt, Parking places[], int n) {
    float carSpeed = 350.0f;
    float newX = carX;
    float newY = carY;

    if (IsKeyDown(KEY_RIGHT)) {
        newX += carSpeed * dt;
        carRotation = -90.0f;
    }
    if (IsKeyDown(KEY_LEFT)) {
        newX -= carSpeed * dt;
        carRotation = 90.0f;
    }
    if (IsKeyDown(KEY_UP)) {
        newY -= carSpeed * dt;
        carRotation = 180.0f;
    }
    if (IsKeyDown(KEY_DOWN)) {
        newY += carSpeed * dt;
        carRotation = 0.0f;
    }

    if (!check_collision_with_parking(newX, newY, places, n)) {
        carX = newX;
        carY = newY;
    }

    delimitation_of_playground();

    collect_and_save_simulation_data_auto(dt);
}
