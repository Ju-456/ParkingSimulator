#include "window_menu.h"

Texture2D background;
Texture2D parking_place;
Texture2D panel_menu;

Texture2D entrance_barrier;
Texture2D entrance_ticket_dispenser;

Texture2D exit_pay_station;
Texture2D exit_barrier;

Texture2D barrier_wall;

Texture2D floor_exit;
Texture2D floor_indicator[3];

Texture2D PC;

carOrientation carOrient[NUM_CARS];

int entranceState = 0; // 0 - closed, 1 - opened
int exitState = 0;
int barrierType = 0; // 0 - entrance, 1 - exit

float entranceAngle = 0.0f;
float exitAngle = 0.0f;
float entranceTargetAngle = 0.0f; // -1.0 - no events pending, >= 0.0 = timer running
float exitTargetAngle = 0.0f;

const float BARRIER_SPEED = 180.0f; // deg/s

bool floorChangeRequestedUp = false;
bool floorChangeRequestedDown = false;

int ticket = 0;  // 0 = not taken, 1 = taken
int payment = 0; // 0 = not paid, 1 = paid

// to open automaticly
double entranceTriggerTime = -1.0; // time when the ticket was taken
double exitTriggerTime = -1.0;     // time when payment was made

// to close automaticly
double entranceOpenTime = -1.0;
double exitOpenTime = -1.0;

// parking floors
int currentFloor = 0;
static const char *FLOOR_FILES[3] = {"graph_floor_0.json", "graph_floor_1.json", "graph_floor_2.json"};
int carFloor = 0;

Rectangle btnRandom;
Rectangle btnManual;
Rectangle btnHardManual;
Rectangle btnReturn;

Rectangle destPreviewLevel;
Rectangle destNextLevel;

static int chosenCar = -1;

Rectangle srcMode = {258, 68, 125, 60};
Rectangle srcArrow = {129, 64, 60, 60};
Rectangle srcReturn = {0, 130, -60, 60};
Rectangle destReturn = {650, 750, 60, 60};
Vector2 origin = {0, 0};
Vector2 origin1 = {60 / 2.0f, 57 / 2.0f};

float carX = 70;
float carY = 73;
float carStep = 15.0f;      // displacement in pixels by pressing
float carRotation = -90.0f; // default to the right

// colors
Color parkingBlue = (Color){72, 153, 175, 255};
Color parkingRed = (Color){255, 13, 91, 255};
Color parkingGreen = (Color){123, 144, 75, 255};
Color brightGreen = (Color){103, 200, 106, 255};
Color brightBlue = (Color){101, 181, 231, 255};

Rectangle btnTicket;
Rectangle btnPay;
bool controlsUnlocked = false;

void draw_parking_places(int n, Parking places[]) {
    const float width = 180.0f;
    const float height = 95.0f;
    const float scale = 0.62f; // 62% of the real size

    for (int i = 0; i < n; i++) {
        float w = width * scale;
        float h = height * scale;

        Rectangle src = (Rectangle){0, 0, width, height};
        Rectangle dest = (Rectangle){places[i].x, places[i].y, w, h};

        if (places[i].direction == 0) { // positive direction
            DrawTexturePro(parking_place, src, dest, (Vector2){0, 0}, 0.0f, WHITE);

        } else { // negative direction => Renversed picture left/right
            Rectangle srcMir = (Rectangle){width, 0, -width, height};
            DrawTexturePro(parking_place, srcMir, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}

void panel() {
    DrawTexture(panel_menu, 0, 560, WHITE);
}

void update_barrier_angles() {
    float delta_time = GetFrameTime(); // elapsed time since the last frame (in seconds)s

    // entrance
    float deltaE = entranceTargetAngle -
                   entranceAngle; // difference between the desired angle and the current angle
    float stepE = BARRIER_SPEED * delta_time; // how much we can rotate this frame

    if (fabsf(deltaE) <= stepE) {
        entranceAngle = entranceTargetAngle; // we can reach the target this frame
    } else {
        entranceAngle += (deltaE > 0 ? stepE : -stepE); // move towards the target
    }

    // exit
    float deltaX = exitTargetAngle - exitAngle;
    float stepX = BARRIER_SPEED * delta_time;

    if (fabsf(deltaX) <= stepX) {
        exitAngle = exitTargetAngle;
    } else {
        exitAngle += (deltaX > 0 ? stepX : -stepX);
    }
}

void barrier_management(int barrierType, int barrier_state) {
    if (barrierType == 0) { // entrance

        if (barrier_state == 0) { // we want to close it
            entranceState = 0;
            entranceTargetAngle = 0.0f;
        } else if (barrier_state == 1) { // we want to open it
            entranceState = 1;
            entranceTargetAngle = -90.0f;
        } else {
            fprintf(stderr, "Wrong barrier state: %d\n", barrier_state);
        }
    } else if (barrierType == 1) { // exit
        if (barrier_state == 0) {  // we want to close it
            exitState = 0;
            exitTargetAngle = 0.0f;
        } else if (barrier_state == 1) { // we want to open it
            exitState = 1;
            exitTargetAngle = 90.0f;
        } else {
            fprintf(stderr, "Wrong barrier state: %d\n", barrier_state);
        }
    } else {
        fprintf(stderr, "Wrong barrier type: %d\n", barrierType);
    }
}

void handle_automatic_opening() {
    if (currentFloor != 0)
        return;
    double now = GetTime();

    // entrance
    // open after taking a ticket
    if (entranceTriggerTime >= 0.0 && (now - entranceTriggerTime) >= OPEN_DELAY) {
        barrier_management(0, 1);   // open
        entranceTriggerTime = -1.0; // no pending events
        entranceOpenTime = now;
    }

    // close after 5sec
    if (entranceOpenTime >= 0.0 && (now - entranceOpenTime) >= CLOSE_DELAY) {
        barrier_management(0, 0); // close
        entranceOpenTime = -1.0;
    }

    // exit
    // open after payement
    if (exitTriggerTime >= 0.0 && (now - exitTriggerTime) >= OPEN_DELAY) {
        barrier_management(1, 1);
        exitTriggerTime = -1.0;
        exitOpenTime = now;
    }

    // close after 5sec
    if (exitOpenTime >= 0.0 && (now - exitOpenTime) >= CLOSE_DELAY) {
        barrier_management(1, 0);
        ticket = 0;
        payment = 0;
        exitOpenTime = -1.0;
    }
}

void draw_entrance_barrier() {
    float x = 140.0f, y = 20.0f;

    DrawTexture(barrier_wall, 10.0f, 103.0f, WHITE);
    DrawTexture(entrance_ticket_dispenser, 100.0f, 20.0f, WHITE);

    Rectangle src =
        (Rectangle){0, 0, (float)entrance_barrier.width, (float)entrance_barrier.height};
    Rectangle dst =
        (Rectangle){x, y, (float)entrance_barrier.width, (float)entrance_barrier.height};

    DrawTexturePro(entrance_barrier, src, dst, origin, entranceAngle, WHITE);
}

void draw_exit_barrier() {
    float x = 645.0f, y = 455.0f;

    DrawTexture(barrier_wall, 660.0f, 445.0f, WHITE);
    DrawTexture(exit_pay_station, 610.0f, 508.0f, WHITE);

    Rectangle src = (Rectangle){0, 0, (float)exit_barrier.width, (float)exit_barrier.height};

    Vector2 origin = {0.0f, (float)exit_barrier.height};
    Rectangle dst = (Rectangle){x + origin.x, y + origin.y, (float)exit_barrier.width,
                                (float)exit_barrier.height};

    DrawTexturePro(exit_barrier, src, dst, origin, exitAngle, WHITE);
}

// reload a floor to reset the states
void reload_floor(int floor, Parking places[], int *num_parking_places) {
    char path[PATH_MAX] = {0};
    build_path(path, "graph_json/", FLOOR_FILES[floor]);

    *num_parking_places = count_number_places(path);
    if (load_graph_from_json(path, *num_parking_places, places)) {
        init_direction_parking_places(*num_parking_places, places);
    } else {
        fprintf(stderr, "[FLOOR] Error loading: %s\n", FLOOR_FILES[floor]);
    }

    entranceState = exitState = 0;
    entranceTargetAngle = exitTargetAngle = 0.0f;
    entranceAngle = exitAngle = 0.0f;
    entranceTriggerTime = exitTriggerTime = -1.0;
    entranceOpenTime = exitOpenTime = -1.0;
}

void draw_floor() {
    // top
    if (currentFloor < 2) {
        DrawTexture(floor_exit, 780.0f, 20.0f, WHITE);
    }
    // bottom
    if (currentFloor > 0) {
        DrawTexture(floor_exit, 10.0f, 460.0f, WHITE);
    }

    DrawTexture(floor_indicator[currentFloor], 740.0f, 20.0f, WHITE);
}

void init_manual_panel_menu() {
    int buttonWidth = srcMode.width;
    int buttonHeight = srcMode.height;
    int PosY = 720;

    btnRandom = (Rectangle){200, PosY, buttonWidth, buttonHeight};
    btnManual = (Rectangle){335, PosY, buttonWidth, buttonHeight};
    btnHardManual = (Rectangle){470, PosY, buttonWidth, buttonHeight};

    btnTicket = (Rectangle){780 - buttonWidth, 580, buttonWidth, buttonHeight};
    btnPay = (Rectangle){780 - buttonWidth, 650, buttonWidth, buttonHeight};

    btnReturn = (Rectangle){120, PosY, 60, 60};
}

void manual_panel_menu(Font font) {
    DrawTextureRec(PC, srcMode, (Vector2){btnRandom.x, btnRandom.y}, WHITE);
    DrawTextEx(font, "Random", (Vector2){btnRandom.x + 28, btnRandom.y + 18}, 18, 1, parkingBlue);

    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y}, WHITE);
    DrawTextEx(font, "Manual", (Vector2){btnManual.x + 32, btnManual.y + 18}, 18, 1, parkingGreen);

    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y}, WHITE);
    // DrawTextEx(font, "Hard Manual", (Vector2){btnHardManual.x + 8, btnHardManual.y + 18}, 18, 1,
    // parkingRed);
    DrawTextEx(font, "     Hard", (Vector2){btnHardManual.x + 8, btnHardManual.y + 18}, 18, 1,
               parkingRed);
}

// void first_random_simulation() {
//     place_car_at_start_pos();
// }
// static inline bool entrance_is_passable() {
// check that the barrier is open and we have a ticket
// return (entranceAngle <= -85.0f) && (ticket == 1);
//}

// static inline bool exit_is_passable() {
//  return (exitAngle >=  85.0f) && (payment == 1);
//}

void load_textures() {

    background = LoadTexture("Assets/background.png");
    parking_place = LoadTexture("Assets/parking_place.png");
    panel_menu = LoadTexture("Assets/panel_menu.png");

    entrance_barrier = LoadTexture("Assets/entrance_barrier.png");
    entrance_ticket_dispenser = LoadTexture("Assets/entrance_ticket_dispenser.png");

    exit_barrier = LoadTexture("Assets/exit_barrier.png");
    exit_pay_station = LoadTexture("Assets/exit_pay_station.png");

    barrier_wall = LoadTexture("Assets/barrier_wall.png");

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
    DrawTexturePro(
        carOrient[CAR_BLACK].right,
        (Rectangle){0, 0, carOrient[CAR_BLACK].right.width, carOrient[CAR_BLACK].right.height},
        (Rectangle){btnRandom.x + 10, btnRandom.y - 65, srcMode.width * scale,
                    srcMode.height * scale},
        (Vector2){0, 0}, 0.0f, WHITE);

    // blue car
    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y - 70}, WHITE);
    DrawTexturePro(
        carOrient[CAR_BLUE].right,
        (Rectangle){0, 0, carOrient[CAR_BLUE].right.width, carOrient[CAR_BLUE].right.height},
        (Rectangle){btnManual.x + 10, btnManual.y - 65, srcMode.width * scale,
                    srcMode.height * scale},
        (Vector2){0, 0}, 0.0f, WHITE);

    // gray car
    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y - 70}, WHITE);
    DrawTexturePro(
        carOrient[CAR_GRAY].right,
        (Rectangle){0, 0, carOrient[CAR_GRAY].right.width, carOrient[CAR_GRAY].right.height},
        (Rectangle){btnHardManual.x + 10, btnHardManual.y - 65, srcMode.width * scale,
                    srcMode.height * scale},
        (Vector2){0, 0}, 0.0f, WHITE);

    // pink car
    DrawTextureRec(PC, srcMode, (Vector2){btnRandom.x, btnRandom.y}, WHITE);
    DrawTexturePro(
        carOrient[CAR_PINK].right,
        (Rectangle){0, 0, carOrient[CAR_PINK].right.width, carOrient[CAR_PINK].right.height},
        (Rectangle){btnRandom.x + 10, btnManual.y, srcMode.width * scale, srcMode.height * scale},
        (Vector2){0, 0}, 0.0f, WHITE);

    // red car
    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y}, WHITE);
    DrawTexturePro(
        carOrient[CAR_RED].right,
        (Rectangle){0, 0, carOrient[CAR_RED].right.width, carOrient[CAR_RED].right.height},
        (Rectangle){btnManual.x + 10, btnManual.y, srcMode.width * scale, srcMode.height * scale},
        (Vector2){0, 0}, 0.0f, WHITE);

    // yellow car
    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y}, WHITE);
    DrawTexturePro(
        carOrient[CAR_YELLOW].right,
        (Rectangle){0, 0, carOrient[CAR_YELLOW].right.width, carOrient[CAR_YELLOW].right.height},
        (Rectangle){btnHardManual.x + 10, btnHardManual.y, srcMode.width * scale,
                    srcMode.height * scale},
        (Vector2){0, 0}, 0.0f, WHITE);

    choose_your_car_condition();
}

void choose_your_car_condition() {
    float scale = 0.75f;

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(
                mouse, (Rectangle){btnRandom.x, btnRandom.y - 70, srcMode.width, srcMode.height})) {
            chosenCar = 0; // black car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnManual.x, btnManual.y - 70,
                                                             srcMode.width, srcMode.height})) {
            chosenCar = 1; // blue car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnHardManual.x, btnHardManual.y - 70,
                                                             srcMode.width, srcMode.height})) {
            chosenCar = 2; // gray car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnRandom.x, btnRandom.y,
                                                             srcMode.width, srcMode.height})) {
            chosenCar = 3; // pink car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnManual.x, btnManual.y,
                                                             srcMode.width, srcMode.height})) {
            chosenCar = 4; // red car
        } else if (CheckCollisionPointRec(mouse, (Rectangle){btnHardManual.x, btnHardManual.y,
                                                             srcMode.width, srcMode.height})) {
            chosenCar = 5; // yellow car
        }
    }

    if (chosenCar == 0) {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(
            carOrient[CAR_BLACK].front,
            (Rectangle){0, 0, carOrient[CAR_BLACK].front.width, carOrient[CAR_BLACK].front.height},
            (Rectangle){65, 583, carOrient[CAR_BLACK].front.width * scale,
                        carOrient[CAR_BLACK].front.height * scale},
            (Vector2){0, 0}, 0.0f, WHITE);

    } else if (chosenCar == 1) {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(
            carOrient[CAR_BLUE].front,
            (Rectangle){0, 0, carOrient[CAR_BLUE].front.width, carOrient[CAR_BLUE].front.height},
            (Rectangle){65, 583, carOrient[CAR_BLUE].front.width * scale,
                        carOrient[CAR_BLUE].front.height * scale},
            (Vector2){0, 0}, 0.0f, WHITE);

    } else if (chosenCar == 2) {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(
            carOrient[CAR_GRAY].front,
            (Rectangle){0, 0, carOrient[CAR_GRAY].front.width, carOrient[CAR_GRAY].front.height},
            (Rectangle){65, 583, carOrient[CAR_GRAY].front.width * scale,
                        carOrient[CAR_GRAY].front.height * scale},
            (Vector2){0, 0}, 0.0f, WHITE);

    } else if (chosenCar == 3) {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(
            carOrient[CAR_PINK].front,
            (Rectangle){0, 0, carOrient[CAR_PINK].front.width, carOrient[CAR_PINK].front.height},
            (Rectangle){65, 583, carOrient[CAR_PINK].front.width * scale,
                        carOrient[CAR_PINK].front.height * scale},
            (Vector2){0, 0}, 0.0f, WHITE);

    } else if (chosenCar == 4) {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(
            carOrient[CAR_RED].front,
            (Rectangle){0, 0, carOrient[CAR_RED].front.width, carOrient[CAR_RED].front.height},
            (Rectangle){65, 583, carOrient[CAR_RED].front.width * scale,
                        carOrient[CAR_RED].front.height * scale},
            (Vector2){0, 0}, 0.0f, WHITE);

    } else if (chosenCar == 5) {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(carOrient[CAR_YELLOW].front,
                       (Rectangle){0, 0, carOrient[CAR_YELLOW].front.width,
                                   carOrient[CAR_YELLOW].front.height},
                       (Rectangle){65, 583, carOrient[CAR_YELLOW].front.width * scale,
                                   carOrient[CAR_YELLOW].front.height * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
}

void update_car_position(float dt) {
    float carSpeed = 350.0f; // num of pixel per second

    if (IsKeyDown(KEY_RIGHT)) {
        carX += carSpeed * dt;
        carRotation = -90.0f;
    }
    if (IsKeyDown(KEY_LEFT)) {
        carX -= carSpeed * dt;
        carRotation = 90.0f;
    }
    if (IsKeyDown(KEY_UP)) {
        carY -= carSpeed * dt;
        carRotation = 180.0f;
    }
    if (IsKeyDown(KEY_DOWN)) {
        carY += carSpeed * dt;
        carRotation = 0.0f;
    }

    delimitation_of_playground();
}

void delimitation_of_playground() {
    if ((carY > 460 && carY < 540) && (carX > 0 && carX < 20)) {
        if (currentFloor > 0) {
            floorChangeRequestedDown = true;
            carFloor--; 
            carX = 710;
            carY = 75;
        }
    }
    if (carY < 0)
        carY = 130; // because height is ± 180 so we're often used 130 for all boundary adjustments
    if (carY > 550)
        carY = 480;
    if (currentFloor == 0) { // barriers wall are managed only on floor P-0
        if ((carY > 100 && carY < 110) && (carX > 0 && carX < 150)) { // First barrier wall
            carY = 100;
        } else if ((carY > 440 && carY < 450) &&
                   (carX > 660 && carX < 750)) { // Second barrier wall
            carY = 400;
        }
    }
    if (carX > 750) {                 // Right boundary
        if (carY > 0 && carY < 100) { // chgmt of level IF != P-2
            if (currentFloor < MAX_FLOOR) {
                floorChangeRequestedUp = true;
                carFloor++; 
                carX = 20;
                carY = 75;
            } else {
                carX = 670;
            }
        } else if (carY >= 100 && carY <= 435) { // 100 < carY < 435
            printf("You have nothing to do here !\n");
            carX = 670;
        } else if (currentFloor != 0) {
            printf("You have nothing to do here bc currentFloor != 0!\n");
            carX = 670;
        }
    } else if (carX < 0) { // Left boundary
        if (carY > 0 && carY < 100) {
            carY = 400;
        } else {
            printf("You have nothing to do here !\n");
            carX = 130;
        }
    }
}

void place_car_at_start_pos() {
    float scale = 0.75f;

    switch (chosenCar) {
    case 0: // black car
        DrawTexturePro(
            carOrient[CAR_BLACK].top,
            (Rectangle){0, 0, carOrient[CAR_BLACK].top.width, carOrient[CAR_BLACK].top.height},
            (Rectangle){carX, carY - 5, carOrient[CAR_BLACK].top.width * 0.7,
                        carOrient[CAR_BLACK].top.height * 1.1},
            (Vector2){carOrient[CAR_BLACK].top.width / 2, carOrient[CAR_BLACK].top.height / 2},
            carRotation, WHITE);
        break;

    case 1: // blue car
        DrawTexturePro(
            carOrient[CAR_BLUE].top,
            (Rectangle){0, 0, carOrient[CAR_BLUE].top.width, carOrient[CAR_BLUE].top.height},
            (Rectangle){carX, carY, carOrient[CAR_BLUE].top.width * scale,
                        carOrient[CAR_BLUE].top.height * scale},
            (Vector2){carOrient[CAR_BLUE].top.width * scale / 2,
                      carOrient[CAR_BLUE].top.height * scale / 2},
            carRotation, WHITE);
        break;

    case 2: // gray car
        DrawTexturePro(
            carOrient[CAR_GRAY].top,
            (Rectangle){0, 0, carOrient[CAR_GRAY].top.width, carOrient[CAR_GRAY].top.height},
            (Rectangle){carX, carY - 5, carOrient[CAR_GRAY].top.width * scale,
                        carOrient[CAR_GRAY].top.height * 1.1},
            (Vector2){carOrient[CAR_GRAY].top.width / 2, carOrient[CAR_GRAY].top.height / 2},
            carRotation, WHITE);
        break;

    case 3: // pink car
        DrawTexturePro(
            carOrient[CAR_PINK].top,
            (Rectangle){0, 0, carOrient[CAR_PINK].top.width, carOrient[CAR_PINK].top.height},
            (Rectangle){carX, carY, carOrient[CAR_PINK].top.width * scale,
                        carOrient[CAR_PINK].top.height},
            (Vector2){carOrient[CAR_PINK].top.width * scale / 2,
                      carOrient[CAR_PINK].top.height * scale / 2},
            carRotation, WHITE);
        break;

    case 4: // red car
        DrawTexturePro(
            carOrient[CAR_RED].top,
            (Rectangle){0, 0, carOrient[CAR_RED].top.width, carOrient[CAR_RED].top.height},
            (Rectangle){carX, carY + 5, carOrient[CAR_RED].top.width,
                        carOrient[CAR_RED].top.height},
            (Vector2){carOrient[CAR_RED].top.width * scale / 2,
                      carOrient[CAR_RED].top.height * scale / 2},
            carRotation, WHITE);
        break;

    case 5: // yellow car
        DrawTexturePro(
            carOrient[CAR_YELLOW].top,
            (Rectangle){0, 0, carOrient[CAR_YELLOW].top.width, carOrient[CAR_YELLOW].top.height},
            (Rectangle){carX, carY, carOrient[CAR_YELLOW].top.width * scale,
                        carOrient[CAR_YELLOW].top.height * scale},
            (Vector2){carOrient[CAR_YELLOW].top.width * scale / 2,
                      carOrient[CAR_YELLOW].top.height * scale / 2},
            carRotation, WHITE);
        break;
    }
}

void draw_buttons_direction(Texture2D PC) {
    Rectangle srcUp = {129, 320, 60, 60};
    Rectangle srcDown = {192, 320, 60, 60};

    Rectangle srcLeft = {256, 320, 60, 60};
    Rectangle srcRight = {325, 320, 60, 60};

    Vector2 posUp = {440, 660};
    Vector2 posDown = {440, 720};

    Vector2 posLeft = {posDown.x - 60, posDown.y};
    Vector2 posRight = {posDown.x + 65, posDown.y};

    DrawTextureRec(PC, srcUp, posUp, WHITE);
    DrawTextureRec(PC, srcDown, posDown, WHITE);
    DrawTextureRec(PC, srcLeft, posLeft, WHITE);
    DrawTextureRec(PC, srcRight, posRight, WHITE);
}

void draw_floor_arrows(Texture2D PC, Rectangle srcArrow, Rectangle prev, Rectangle next, int floor, bool enabled) {
    Vector2 origin = {srcArrow.width / 2.0f, srcArrow.height / 2.0f};

    bool canUp = enabled && (floor < MAX_FLOOR);
    bool canDown = enabled && (floor > 0);

    Color upTint = canUp ? WHITE : Fade(GRAY, 0.5f);
    Color downTint = canDown ? WHITE : Fade(GRAY, 0.5f);

    DrawTexturePro(PC, srcArrow, prev, origin, 0.0f, upTint);
    DrawTexturePro(PC, srcArrow, next, origin, 180.0f, downTint);
}

void draw_return_arrow(Texture2D PC, Rectangle srcReturn, Rectangle destReturn, bool enabled) {

    Color returnColor = enabled ? brightBlue : Fade(GRAY, 0.5f);

    DrawTexturePro(PC, srcReturn, destReturn, origin1, 0, returnColor);
}

static inline bool game_mode_selected(Screen s) {
    return s == SCREEN_RANDOM || s == SCREEN_MANUAL || s == SCREEN_HARD_MANUAL ||
           s == SCREEN_DIRECTION;
}

static inline Color disabled_tint(Color base, bool enabled) {
    return enabled ? base : Fade(GRAY, 0.5f);
}

void draw_ticket_pay_buttons(Font font, bool enabled) {
    bool canUse = (currentFloor == 0) && enabled;
    Color btnTint = disabled_tint(WHITE, canUse);
    Color textTint1 = disabled_tint(parkingBlue, canUse);
    Color textTint2 = disabled_tint(parkingRed, canUse);

    DrawTextureRec(PC, srcMode, (Vector2){btnTicket.x, btnTicket.y}, btnTint);
    DrawTextEx(font, "Ticket", (Vector2){btnTicket.x + 32, btnTicket.y + 18}, 18, 1, textTint1);

    DrawTextureRec(PC, srcMode, (Vector2){btnPay.x, btnPay.y}, btnTint);
    DrawTextEx(font, "  Pay", (Vector2){btnPay.x + 35, btnPay.y + 18}, 18, 1, textTint2);
}

void handle_station_buttons_click(Vector2 mouse, bool enabled) {
    if (!enabled)
        return;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (currentFloor != 0) {
            if (CheckCollisionPointRec(mouse, btnTicket) || CheckCollisionPointRec(mouse, btnPay)) {
                printf("[FLOOR] Take the ticket or pay only on the ground floor (0).\n");
            }
            return;
        }

        if (CheckCollisionPointRec(mouse, btnTicket)) {
            if (ticket == 0) {
                ticket = 1;
                entranceTriggerTime = GetTime();
                printf("[ENTRY] Ticket taken.\n");
            } else {
                printf("[ENTRY] Ticket was already taken.\n");
            }
        }

        if (CheckCollisionPointRec(mouse, btnPay)) {
            if (ticket == 0) {
                printf("[EXIT] Unable to pay: no ticket.\n");
                fflush(stdout);
            } else if (payment == 0) {
                payment = 1;
                exitTriggerTime = GetTime();
                printf("[EXIT] Ticket paid.\n");
            } else {
                printf("[EXIT] Ticket was already paid.\n");
            }
        }
    }
}

void unload_textures() {
    UnloadTexture(background);
    UnloadTexture(parking_place);
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

    UnloadTexture(PC);

    for (int i = 0; i < NUM_CARS; i++) {
        UnloadTexture(carOrient[i].front);
        UnloadTexture(carOrient[i].right);
        UnloadTexture(carOrient[i].top);
    }
}

void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[]) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Parking Simulator");
    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    int posX = (monitorWidth - SCREEN_WIDTH) / 2;
    int posY = (monitorHeight - SCREEN_HEIGHT) / 2;

    SetWindowPosition(posX, posY);
    SetTargetFPS(60);

    Font font = LoadFontEx("resources/Minecraftia.ttf", 15, NULL, 0);

    currentFloor = 0;
    load_textures();

    const char *message1 = "Welcome to our Parking Simulator";
    const char *message2 = "Choose a Game Mode :";

    int letters1 = 0;
    float timer1 = 0.0f;

    int letters2 = 0;
    float letterTimer2 = 0.0f;

    float letterDelay = 0.05f;

    Screen currentScreen = SCREEN_MANUAL_PANEL;

    destPreviewLevel = (Rectangle){750, 750, srcArrow.width, srcArrow.height};
    destNextLevel = (Rectangle){50, 750, srcArrow.width, srcArrow.height};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(background, 0, 0, WHITE);

        panel();
        draw_ticket_pay_buttons(font, controlsUnlocked);
        if (currentFloor == 0) {
            draw_entrance_barrier();
            draw_exit_barrier();
        }
        draw_parking_places(num_parking_places, places);
        draw_floor();

        // permanent arrows (independently of the mode)
        Rectangle srcArrow = {129, 64, 60, 60};

        bool floorsEnabled = controlsUnlocked;
        draw_floor_arrows(PC, srcArrow, destPreviewLevel, destNextLevel, currentFloor,
                          floorsEnabled);

        Rectangle srcReturn = {0, 130, 65, 60};
        Rectangle destReturn = {140, 750, srcArrow.width, srcArrow.height};
        draw_return_arrow(PC, srcReturn, destReturn, game_mode_selected(currentScreen));

        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();
        DrawText(TextFormat("(x;y) = (%d;%d)", (int)mouse.x, (int)mouse.y), 10, 10, 12, BLACK);
        // to change floors
        if (controlsUnlocked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, destPreviewLevel)) {
                if (currentFloor < MAX_FLOOR) {
                    currentFloor++;
                    reload_floor(currentFloor, places, &num_parking_places);
                } else {
                    printf("[FLOOR] Already on the top floor (2)\n");
                }
            } else if (CheckCollisionPointRec(mouse, destNextLevel)) {
                if (currentFloor > 0) {
                    currentFloor--;
                    reload_floor(currentFloor, places, &num_parking_places);
                } else {
                    printf("[FLOOR] Already on the ground floor (0)\n");
                }
            }
        }
        if (floorChangeRequestedUp) {
            currentFloor++;
            reload_floor(currentFloor, places, &num_parking_places);
            floorChangeRequestedUp = false;
        }
        if (floorChangeRequestedDown) {
            currentFloor--;
            reload_floor(currentFloor, places, &num_parking_places);
            floorChangeRequestedDown = false;
        }
        update_barrier_angles();
        handle_station_buttons_click(mouse, controlsUnlocked);
        handle_automatic_opening();

        switch (currentScreen) {
        case SCREEN_MANUAL_PANEL:
            controlsUnlocked = false;
            if (letters1 < strlen(message1)) {
                timer1 += dt;
                if (timer1 >= letterDelay) {
                    letters1++;
                    timer1 = 0.0f;
                }
            } else {
                if (letters2 < strlen(message2)) {
                    letterTimer2 += dt;
                    if (letterTimer2 >= letterDelay) {
                        letters2++;
                        letterTimer2 = 0.0f;
                    }
                }
            }

            DrawTextEx(font, TextSubtext(message1, 0, letters1), (Vector2){200, 590}, 24, 2, WHITE);
            DrawTextEx(font, TextSubtext(message2, 0, letters2), (Vector2){280, 620}, 22, 2, WHITE);

            init_manual_panel_menu();
            manual_panel_menu(font);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btnRandom))
                    currentScreen = SCREEN_RANDOM;
                if (CheckCollisionPointRec(mouse, btnManual))
                    currentScreen = SCREEN_MANUAL;
                if (CheckCollisionPointRec(mouse, btnHardManual))
                    currentScreen = SCREEN_HARD_MANUAL;
            }
            break;

        case SCREEN_RANDOM:
            DrawText("Random mode", 200, 400, 20, parkingBlue);
            if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, btnReturn))) {
                controlsUnlocked = true;
                currentScreen = SCREEN_MANUAL_PANEL;
                if (currentFloor != 0) {
                    currentFloor = 0;
                    reload_floor(currentFloor, places, &num_parking_places);
                }
            }
            break;

        case SCREEN_MANUAL:
            choose_your_car(font);
            if (currentFloor == carFloor) {
                place_car_at_start_pos();
            }

            if (chosenCar != -1) {
                Rectangle srcReturn1 = {0, 130, -65, 60};
                Rectangle destNextStep = {625, 725, srcArrow.width, srcArrow.height};
                DrawTexturePro(PC, srcReturn1, destNextStep, origin, 0, brightGreen);
                // DrawRectangleLines((int)destNextStep.x, (int)destNextStep.y,
                // (int)destNextStep.width, (int)destNextStep.height, RED);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                    CheckCollisionPointRec(mouse, destNextStep)) {
                    controlsUnlocked = true;
                    currentScreen = SCREEN_DIRECTION;
                    carFloor = currentFloor;
                }
            }

            if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, btnReturn))) {
                currentScreen = SCREEN_MANUAL_PANEL;
                place_car_at_start_pos();
                if (currentFloor != 0) {
                    currentFloor = 0;
                    reload_floor(currentFloor, places, &num_parking_places);
                }
            }
            break;

        case SCREEN_DIRECTION:
            if (currentFloor == carFloor) {
                update_car_position(dt);
                place_car_at_start_pos();
            }

            if ((CheckCollisionPointRec(mouse, btnReturn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
                controlsUnlocked = false;
                currentScreen = SCREEN_MANUAL;
                place_car_at_start_pos();
                if (currentFloor != 0) {
                    currentFloor = 0;
                    reload_floor(currentFloor, places, &num_parking_places);
                }
            }
            break;

        case SCREEN_HARD_MANUAL:
            DrawText("Hard Manual mode", 200, 400, 20, parkingRed);
            if ((CheckCollisionPointRec(mouse, btnReturn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
                controlsUnlocked = true;
                place_car_at_start_pos();
                currentScreen = SCREEN_MANUAL_PANEL;
            }
            break;
        }

        EndDrawing();
    }

    UnloadFont(font);
    unload_textures();
    CloseWindow();
}
