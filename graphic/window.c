#include "window.h"

Texture2D background;
Texture2D free_parking_place;
Texture2D busy_parking_place;
Texture2D panel_menu;

Texture2D entrance_barrier;
Texture2D entrance_ticket_dispenser;

Texture2D exit_pay_station;
Texture2D exit_barrier;

Texture2D barrier_wall;

Texture2D floor_exit;
Texture2D floor_indicator[3];

Texture2D PC;

Texture2D car_preview;

carOrientation carOrient[NUM_CARS];

bool gameFinished = false;
bool insideParking = false;

bool floorChangeRequestedUp = false;
bool floorChangeRequestedDown = false;

float entranceAngle = 0.0f;
float entranceTargetAngle = 0.0f;
float exitAngle = 0.0f;
float exitTargetAngle = 0.0f;

int entranceState = 0;
int exitState = 0;

double entranceTriggerTime = -1.0;
double exitTriggerTime = -1.0;

double entranceOpenTime = -1.0;
double exitOpenTime = -1.0;

int ticket = 0;
int payment = 0;

int currentFloor = 0;
int carFloor = 0;

Rectangle btnRandom;
Rectangle btnManual;
Rectangle btnHardManual;
Rectangle btnReturn;

Rectangle destPreviewLevel;
Rectangle destNextLevel;

int chosenCar = -1;

Rectangle srcMode = {258, 68, 125, 60};
Rectangle srcPreview = {258, 68, 150, 80};
Rectangle srcArrow = {129, 64, 60, 60};
Rectangle srcReturn = {0, 130, -60, 60};
Rectangle destReturn = {650, 750, 60, 60};
Vector2 origin = {0, 0};
Vector2 origin1 = {60 / 2.0f, 57 / 2.0f};

float carX;
float carY;
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

// Global simple simulation state
bool randomSimulationStarted = false;
bool simPendingStart = false;
bool simRunning = false;

Color disabled_tint(Color base, bool enabled) {
    return enabled ? base : Fade(GRAY, 0.5f);
}

bool game_mode_selected(Screen s) {
    return s == SCREEN_RANDOM || s == SCREEN_MANUAL || s == SCREEN_HARD_MANUAL || s == SCREEN_DIRECTION;
}

void place_car_at_start_pos() {
    float scale = 0.65f;

    switch (chosenCar) {
    case 0: // black car
        DrawTexturePro(carOrient[CAR_BLACK].top, (Rectangle){0, 0, carOrient[CAR_BLACK].top.width, carOrient[CAR_BLACK].top.height},
                       (Rectangle){carX, carY - 5, carOrient[CAR_BLACK].top.width * scale, carOrient[CAR_BLACK].top.height * 1},
                       (Vector2){carOrient[CAR_BLACK].top.width / 2, carOrient[CAR_BLACK].top.height / 2}, carRotation, WHITE);
        break;

    case 1: // blue car
        DrawTexturePro(carOrient[CAR_BLUE].top, (Rectangle){0, 0, carOrient[CAR_BLUE].top.width, carOrient[CAR_BLUE].top.height},
                       (Rectangle){carX, carY, carOrient[CAR_BLUE].top.width * 0.65f, carOrient[CAR_BLUE].top.height * 0.65f},
                       (Vector2){carOrient[CAR_BLUE].top.width * 0.65f / 2, carOrient[CAR_BLUE].top.height * 0.65f / 2}, carRotation, WHITE);
        break;

    case 2: // gray car
        DrawTexturePro(carOrient[CAR_GRAY].top, (Rectangle){0, 0, carOrient[CAR_GRAY].top.width, carOrient[CAR_GRAY].top.height},
                       (Rectangle){carX, carY - 5, carOrient[CAR_GRAY].top.width * scale, carOrient[CAR_GRAY].top.height * 1},
                       (Vector2){carOrient[CAR_GRAY].top.width / 2, carOrient[CAR_GRAY].top.height / 2}, carRotation, WHITE);
        break;

    case 3: // pink car
        DrawTexturePro(carOrient[CAR_PINK].top, (Rectangle){0, 0, carOrient[CAR_PINK].top.width, carOrient[CAR_PINK].top.height},
                       (Rectangle){carX, carY, carOrient[CAR_PINK].top.width * scale, carOrient[CAR_PINK].top.height * 0.9},
                       (Vector2){carOrient[CAR_PINK].top.width * scale / 2, carOrient[CAR_PINK].top.height * scale / 2}, carRotation, WHITE);
        break;

    case 4: // red car
        DrawTexturePro(carOrient[CAR_RED].top, (Rectangle){0, 0, carOrient[CAR_RED].top.width, carOrient[CAR_RED].top.height},
                       (Rectangle){carX, carY + 5, carOrient[CAR_RED].top.width * 0.9f, carOrient[CAR_RED].top.height * 0.9f},
                       (Vector2){carOrient[CAR_RED].top.width * scale / 2, carOrient[CAR_RED].top.height * scale / 2}, carRotation, WHITE);
        break;

    case 5: // yellow car
        DrawTexturePro(carOrient[CAR_YELLOW].top, (Rectangle){0, 0, carOrient[CAR_YELLOW].top.width, carOrient[CAR_YELLOW].top.height},
                       (Rectangle){carX, carY, carOrient[CAR_YELLOW].top.width * scale, carOrient[CAR_YELLOW].top.height * scale},
                       (Vector2){carOrient[CAR_YELLOW].top.width * scale / 2, carOrient[CAR_YELLOW].top.height * scale / 2}, carRotation, WHITE);
        break;
    }
}

void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[]) {
    SetTraceLogLevel(LOG_NONE);
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
    init_parking_state(num_parking_places, places);
    reload_floor(currentFloor, places, &num_parking_places);

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
        draw_parked_cars(num_parking_places, places);
        draw_floor();

        // permanent arrows (independently of the mode)
        Rectangle srcArrow = {129, 64, 60, 60};

        bool floorsEnabled = controlsUnlocked;
        draw_floor_arrows(PC, srcArrow, destPreviewLevel, destNextLevel, currentFloor, floorsEnabled);

        Rectangle srcReturn = {0, 130, 65, 60};
        Rectangle destReturn = {140, 750, srcArrow.width, srcArrow.height};
        draw_return_arrow(PC, srcReturn, destReturn, game_mode_selected(currentScreen));

        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();
        DrawText(TextFormat("(x;y) = (%d;%d)", (int)mouse.x, (int)mouse.y), 10, 10, 12, BLACK);
        // to change floors
        if (controlsUnlocked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, destPreviewLevel)) {
                request_floor_change(1, places, &num_parking_places);
            } else if (CheckCollisionPointRec(mouse, destNextLevel)) {
                request_floor_change(-1, places, &num_parking_places);
            }
        }
        if (floorChangeRequestedUp) {
            request_floor_change(1, places, &num_parking_places);
            floorChangeRequestedUp = false;
        }
        if (floorChangeRequestedDown) {
            request_floor_change(-1, places, &num_parking_places);
            floorChangeRequestedDown = false;
        }
        update_barrier_angles();
        handle_station_buttons_click(mouse, controlsUnlocked);
        handle_automatic_opening();
        if (gameFinished && currentScreen != SCREEN_END) {
            currentScreen = SCREEN_END;
        }

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
            if (!randomSimulationStarted) {
                select_random_car();
                place_car_at_start_pos();

                int chosenSim = rand() % 6;
                request_simulation_start(chosenSim);

                randomSimulationStarted = true;
                printf("randomSimulationStarted = %d, simRequested = %d\n", randomSimulationStarted, chosenSim);
            }

            float dt_sim = GetFrameTime();

            update_barrier_angles();
            handle_automatic_opening();

            update_simulation(dt_sim); // Update simulation (reads positions from file)
            delimitation_of_playground();

            if (currentFloor != carFloor) { // Ensure (currentFloor == carFloor) => so car is always visible !!!
                currentFloor = carFloor;
                reload_floor(currentFloor, places, &num_parking_places);
            }

            if (currentFloor == carFloor) { // Draw car if on current floor
                place_car_at_start_pos();
            }

            if (is_replay_finished() && randomSimulationStarted) {
                randomSimulationStarted = false;
                simRunning = false;
                simPendingStart = false;
                stop_replay_file(); // ensure closed
                controlsUnlocked = false;
                currentScreen = SCREEN_END;
                printf("Randow replay finished direction to 'SCREEN_END'\n");
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, btnReturn)) {
                simRunning = false;
                simPendingStart = false;
                randomSimulationStarted = false;

                controlsUnlocked = true;
                currentScreen = SCREEN_MANUAL_PANEL;

                if (currentFloor != 0) { // in case !
                    currentFloor = 0;
                    reload_floor(currentFloor, places, &num_parking_places);
                }
            }
            break;

        case SCREEN_END:
            controlsUnlocked = false;

            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
            DrawText("*** Thank you for playing Parking Simulator! ***", 160, 300, 20, WHITE);
            DrawText("Press [Enter] to return to the menu", 180, 350, 20, WHITE);

            static float blink = 0;
            blink += 0.05f;
            if (sin(blink) > 0) {
                DrawText("--> Replay <--", 320, 420, 24, SKYBLUE);
            }
            if (IsKeyDown(KEY_ENTER)) {
                currentScreen = SCREEN_MANUAL_PANEL;
                reset_parking_state(places, &num_parking_places);
            }

            break;

        case SCREEN_MANUAL:
            choose_your_car(font);
            if (currentFloor == carFloor) {
                carX = 70;
                carY = 73;
                place_car_at_start_pos();
            }

            if (chosenCar != -1) {
                Rectangle srcReturn1 = {0, 130, -65, 60};
                Rectangle destNextStep = {625, 725, srcArrow.width, srcArrow.height};
                DrawTexturePro(PC, srcReturn1, destNextStep, origin, 0, brightGreen);
                // DrawRectangleLines((int)destNextStep.x, (int)destNextStep.y,
                // (int)destNextStep.width, (int)destNextStep.height, RED);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, destNextStep)) {
                    controlsUnlocked = true;
                    currentScreen = SCREEN_DIRECTION;
                    carFloor = currentFloor;
                }
            }

            if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, btnReturn))) {
                currentScreen = SCREEN_MANUAL_PANEL;
                reset_parking_state(places, &num_parking_places);
            }
            break;

        case SCREEN_DIRECTION:
            if (currentFloor == carFloor) {
                update_car_position(dt, places, num_parking_places);
                place_car_at_start_pos();
            }

            if ((CheckCollisionPointRec(mouse, btnReturn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
                currentScreen = SCREEN_MANUAL_PANEL;
                reset_parking_state(places, &num_parking_places);
            }
            break;

        case SCREEN_HARD_MANUAL:
            DrawText("Hard Manual mode", 200, 400, 20, parkingRed);
            if ((CheckCollisionPointRec(mouse, btnReturn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
                currentScreen = SCREEN_MANUAL_PANEL;
                reset_parking_state(places, &num_parking_places);
            }
            break;
        }

        EndDrawing();
    }

    UnloadFont(font);
    unload_textures();
    CloseWindow();
}
