#include "window.h"

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
    // to avoid collision with occupied parking places
    if (!check_collision_with_parking(newX, newY, places, n)) {
        carX = newX;
        carY = newY;
    }

    delimitation_of_playground();
}
