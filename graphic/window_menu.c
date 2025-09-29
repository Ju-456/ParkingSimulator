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

Texture2D blackRightTex, blackFrontTex, blackTopTex;
Texture2D blueRightTex, blueFrontTex, blueTopTex;
Texture2D grayRightTex, grayFrontTex, grayTopTex;
Texture2D pinkRightTex, pinkFrontTex, pinkTopTex;
Texture2D redRightTex, redFrontTex, redTopTex;
Texture2D yellowRightTex, yellowFrontTex, yellowTopTex;

int entrance_state = 0; // 0 - closed, 1 - opened
int exit_state = 0;
int barrier_type = 0; // 0 - entrance, 1 - exit

float entrance_angle = 0.0f;
float exit_angle = 0.0f;
float entrance_target_angle = 0.0f; // -1.0 - no events pending, >= 0.0 = timer running
float exit_target_angle = 0.0f;

const float BARRIER_SPEED = 180.0f; // deg/s

int ticket = 0;  // 0 = not taken, 1 = taken
int payment = 0; // 0 = not paid, 1 = paid

// to open automaticly
double entrance_trigger_time = -1.0; // time when the ticket was taken
double exit_trigger_time = -1.0;     // time when payment was made
const double OPEN_DELAY = 2.0;

// to close automaticly
double entrance_open_time = -1.0;
double exit_open_time = -1.0;
const double CLOSE_DELAY = 5.0;

// parking floors
int current_floor = 0;
static const char *FLOOR_FILES[3] = {
    "graph_floor_0.json", "graph_floor_1.json", "graph_floor_2.json"};

Rectangle btnRandom;
Rectangle btnManual;
Rectangle btnHardManual;
Rectangle btnReturn;

Rectangle destPreviewLevel;
Rectangle destNextLevel;

static int chosenCar = -1;
Rectangle srcMode = {258, 65, 125, 60};
Rectangle srcArrow = {129, 64, 60, 57};
Rectangle srcReturn = {0, 130, -60, 60};
Rectangle destReturn = {650, 750, 60, 57};
Vector2 origin = {0, 0};
Vector2 origin1 = {60 / 2.0f, 57 / 2.0f};

float carX = 70;
float carY = 73;
float carStep = 15.0f;      // displacement in pixels by pressing
float carRotation = -90.0f; // default to the right

void draw_parking_places(int n, Parking places[])
{
    const float width = 180.0f;
    const float height = 95.0f;
    const float scale = 0.62f; // 62% of the real size

    for (int i = 0; i < n; i++)
    {
        float w = width * scale;
        float h = height * scale;

        Rectangle src = (Rectangle){0, 0, width, height};
        Rectangle dest = (Rectangle){places[i].x, places[i].y, w, h};

        if (places[i].direction == 0)
        { // positive direction
            DrawTexturePro(parking_place, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else
        { // negative direction => Renversed picture left/right
            Rectangle srcMir = (Rectangle){width, 0, -width, height};
            DrawTexturePro(parking_place, srcMir, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}

void panel()
{
    DrawTexture(panel_menu, 0, 560, WHITE);
}

void update_barrier_angles()
{
    float delta_time = GetFrameTime(); // elapsed time since the last frame (in seconds)s

    // entrance
    float deltaE = entrance_target_angle - entrance_angle; // difference between the desired angle and the current angle
    float stepE = BARRIER_SPEED * delta_time;              // how much we can rotate this frame

    if (fabsf(deltaE) <= stepE)
    {
        entrance_angle = entrance_target_angle; // we can reach the target this frame
    }
    else
    {
        entrance_angle += (deltaE > 0 ? stepE : -stepE); // move towards the target
    }

    // exit
    float deltaX = exit_target_angle - exit_angle;
    float stepX = BARRIER_SPEED * delta_time;

    if (fabsf(deltaX) <= stepX)
    {
        exit_angle = exit_target_angle;
    }
    else
    {
        exit_angle += (deltaX > 0 ? stepX : -stepX);
    }
}

void barrier_management(int barrier_type, int barrier_state)
{
    if (barrier_type == 0) // entrance
    {
        if (barrier_state == 0) // we want to close it
        {
            entrance_state = 0;
            entrance_target_angle = 0.0f;
        }
        else if (barrier_state == 1) // we want to open it
        {
            entrance_state = 1;
            entrance_target_angle = -90.0f;
        }
        else
        {
            fprintf(stderr, "Wrong barrier state: %d\n", barrier_state);
        }
    }
    else if (barrier_type == 1) // exit
    {
        if (barrier_state == 0) // we want to close it
        {
            exit_state = 0;
            exit_target_angle = 0.0f;
        }
        else if (barrier_state == 1) // we want to open it
        {
            exit_state = 1;
            exit_target_angle = 90.0f;
        }
        else
        {
            fprintf(stderr, "Wrong barrier state: %d\n", barrier_state);
        }
    }
    else
    {
        fprintf(stderr, "Wrong barrier type: %d\n", barrier_type);
    }
}

void handle_stations_input()
{
    if (current_floor != 0)
    {
        if (IsKeyPressed(KEY_T) || IsKeyPressed(KEY_P))
        {
            printf("[FLOOR] Take the ticket or pay only on the ground floor (0).\n");
        }
        return;
    }
    // take the ticket at the entrance
    if (IsKeyPressed(KEY_T))
    {
        if (ticket == 0)
        {
            ticket = 1;
            entrance_trigger_time = GetTime();
            printf("[ENTRY] Ticket taken.\n");
        }
        else
        {
            printf("[ENTRY] Ticket was already taken.\n");
        }
    }

    // pay for the ticket on exit
    if (IsKeyPressed(KEY_P))
    {
        if (ticket == 0)
        {
            printf("[EXIT] Unable to pay: no ticket.\n");
            fflush(stdout);
        }
        else if (payment == 0)
        {
            payment = 1;
            exit_trigger_time = GetTime();
            printf("[EXIT] Ticket paid.\n");
        }
        else
        {
            printf("[EXIT] Ticket was already paid.\n");
        }
    }
}

void handle_automatic_opening()
{
    if (current_floor != 0)
        return;
    double now = GetTime();

    // entrance
    // open after taking a ticket
    if (entrance_trigger_time >= 0.0 && (now - entrance_trigger_time) >= OPEN_DELAY)
    {
        barrier_management(0, 1);     // open
        entrance_trigger_time = -1.0; // no pending events
        entrance_open_time = now;
    }

    // close after 5sec
    if (entrance_open_time >= 0.0 && (now - entrance_open_time) >= CLOSE_DELAY)
    {
        barrier_management(0, 0); // close
        entrance_open_time = -1.0;
    }

    // exit
    // open after payement
    if (exit_trigger_time >= 0.0 && (now - exit_trigger_time) >= OPEN_DELAY)
    {
        barrier_management(1, 1);
        exit_trigger_time = -1.0;
        exit_open_time = now;
    }

    // close after 5sec
    if (exit_open_time >= 0.0 && (now - exit_open_time) >= CLOSE_DELAY)
    {
        barrier_management(1, 0);
        ticket = 0;
        payment = 0;
        exit_open_time = -1.0;
    }
}

void draw_entrance_barrier()
{
    float x = 140.0f, y = 20.0f;

    DrawTexture(barrier_wall, 10.0f, 103.0f, WHITE);
    DrawTexture(entrance_ticket_dispenser, 100.0f, 20.0f, WHITE);

    Rectangle src = (Rectangle){0, 0, (float)entrance_barrier.width, (float)entrance_barrier.height};
    Rectangle dst = (Rectangle){x, y, (float)entrance_barrier.width, (float)entrance_barrier.height};

    DrawTexturePro(entrance_barrier, src, dst, origin, entrance_angle, WHITE);
}

void draw_exit_barrier()
{
    float x = 645.0f, y = 455.0f;

    DrawTexture(barrier_wall, 660.0f, 445.0f, WHITE);
    DrawTexture(exit_pay_station, 610.0f, 508.0f, WHITE);

    Rectangle src = (Rectangle){0, 0, (float)exit_barrier.width, (float)exit_barrier.height};

    Vector2 origin = {0.0f, (float)exit_barrier.height};
    Rectangle dst = (Rectangle){x + origin.x, y + origin.y, (float)exit_barrier.width, (float)exit_barrier.height};

    DrawTexturePro(exit_barrier, src, dst, origin, exit_angle, WHITE);
}

// reload a floor to reset the states
void reload_floor(int floor, Parking places[], int *num_parking_places)
{
    char path[PATH_MAX] = {0};
    build_path(path, "graph_json/", FLOOR_FILES[floor]);

    *num_parking_places = count_number_places(path);
    if (load_graph_from_json(path, *num_parking_places, places))
    {
        init_direction_parking_places(*num_parking_places, places);
    }
    else
    {
        fprintf(stderr, "[FLOOR] Error loading: %s\n", FLOOR_FILES[floor]);
    }

    entrance_state = exit_state = 0;
    entrance_target_angle = exit_target_angle = 0.0f;
    entrance_angle = exit_angle = 0.0f;
    entrance_trigger_time = exit_trigger_time = -1.0;
    entrance_open_time = exit_open_time = -1.0;
}
// old verison
void handle_floor_input(Parking places[], int *num_parking_places)
{
    // UP
    if (IsKeyPressed(KEY_U))
    {
        if (current_floor < 2)
        {
            current_floor++;
            reload_floor(current_floor, places, num_parking_places);
        }
        else
        {
            printf("[FLOOR] Already on the top floor (2)\n");
        }
    }

    // DOWN
    if (IsKeyPressed(KEY_D))
    {
        if (current_floor > 0)
        {
            current_floor--;
            reload_floor(current_floor, places, num_parking_places);
        }
        else
        {
            printf("[FLOOR] Already on the ground floor (0)\n");
        }
    }
}

void draw_floor()
{
    DrawTexture(floor_exit, 780.0f, 20.0f, WHITE);
    DrawTexture(floor_indicator[current_floor], 740.0f, 20.0f, WHITE);
}

void init_ordored_panel_menu()
{
    Rectangle srcMode = {258, 65, 125, 60};
    int buttonWidth = srcMode.width;
    int buttonHeight = srcMode.height;
    int PosY = 720;

    btnRandom = (Rectangle){200, PosY, buttonWidth, buttonHeight};
    btnManual = (Rectangle){335, PosY, buttonWidth, buttonHeight};
    btnHardManual = (Rectangle){470, PosY, buttonWidth, buttonHeight};

    btnReturn = (Rectangle){120, PosY, 60, 57};
}

void ordored_panel_menu(Font font)
{
    Rectangle srcMode = {258, 65, 125, 60};
    DrawTextureRec(PC, srcMode, (Vector2){btnRandom.x, btnRandom.y}, WHITE);
    DrawTextEx(font, "Random", (Vector2){btnRandom.x + 28, btnRandom.y + 18}, 18, 1, DARKBLUE);

    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y}, WHITE);
    DrawTextEx(font, "Manual", (Vector2){btnManual.x + 32, btnManual.y + 18}, 18, 1, DARKGREEN);

    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y}, WHITE);
    DrawTextEx(font, "Hard Manual", (Vector2){btnHardManual.x + 8, btnHardManual.y + 18}, 18, 1, MAROON);
}

void choose_your_car(Font font)
{
    const char *message = "Choose a car :";

    static int letters = 0;
    static float timer = 0.0f;

    float letterDelay = 0.07f;
    float dt = GetFrameTime();

    if (letters < strlen(message))
    {
        timer += dt;
        if (timer >= letterDelay)
        {
            letters++;
            timer = 0.0f;
        }
    }

    DrawTextEx(font, TextSubtext(message, 0, letters), (Vector2){300, 590}, 24, 2, WHITE);

    Rectangle srcMode = {258, 65, 125, 60};

    float scale = 0.8f;

    // black car
    DrawTextureRec(PC, srcMode, (Vector2){btnRandom.x, btnRandom.y - 70}, WHITE);
    DrawTexturePro(blackRightTex, (Rectangle){0, 0, blackRightTex.width, blackRightTex.height},
                   (Rectangle){btnRandom.x + 10, btnRandom.y - 65, srcMode.width * scale, srcMode.height * scale},
                   (Vector2){0, 0}, 0.0f, WHITE);

    // blue car
    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y - 70}, WHITE);
    DrawTexturePro(blueRightTex, (Rectangle){0, 0, blueRightTex.width, blueRightTex.height},
                   (Rectangle){btnManual.x + 10, btnManual.y - 65, srcMode.width * scale, srcMode.height * scale},
                   (Vector2){0, 0}, 0.0f, WHITE);

    // gray car
    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y - 70}, WHITE);
    DrawTexturePro(grayRightTex, (Rectangle){0, 0, grayRightTex.width, grayRightTex.height},
                   (Rectangle){btnHardManual.x + 10, btnHardManual.y - 65, srcMode.width * scale, srcMode.height * scale},
                   (Vector2){0, 0}, 0.0f, WHITE);

    // pink car
    DrawTextureRec(PC, srcMode, (Vector2){btnRandom.x, btnRandom.y}, WHITE);
    DrawTexturePro(pinkRightTex, (Rectangle){0, 0, pinkRightTex.width, pinkRightTex.height},
                   (Rectangle){btnRandom.x + 10, btnManual.y, srcMode.width * scale, srcMode.height * scale},
                   (Vector2){0, 0}, 0.0f, WHITE);

    // red car
    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y}, WHITE);
    DrawTexturePro(redRightTex, (Rectangle){0, 0, redRightTex.width, redRightTex.height},
                   (Rectangle){btnManual.x + 10, btnManual.y, srcMode.width * scale, srcMode.height * scale},
                   (Vector2){0, 0}, 0.0f, WHITE);

    // yellow car
    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y}, WHITE);
    DrawTexturePro(yellowRightTex, (Rectangle){0, 0, yellowRightTex.width, yellowRightTex.height},
                   (Rectangle){btnHardManual.x + 10, btnHardManual.y, srcMode.width * scale, srcMode.height * scale},
                   (Vector2){0, 0}, 0.0f, WHITE);

    choose_your_car_condition();
}

void choose_your_car_condition()
{
    float scale = 0.75f;

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse, (Rectangle){btnRandom.x, btnRandom.y - 70, srcMode.width, srcMode.height}))
        {
            chosenCar = 0; // black car
        }
        else if (CheckCollisionPointRec(mouse, (Rectangle){btnManual.x, btnManual.y - 70, srcMode.width, srcMode.height}))
        {
            chosenCar = 1; // blue car
        }
        else if (CheckCollisionPointRec(mouse, (Rectangle){btnHardManual.x, btnHardManual.y - 70, srcMode.width, srcMode.height}))
        {
            chosenCar = 2; // gray car
        }
        else if (CheckCollisionPointRec(mouse, (Rectangle){btnRandom.x, btnRandom.y, srcMode.width, srcMode.height}))
        {
            chosenCar = 3; // pink car
        }
        else if (CheckCollisionPointRec(mouse, (Rectangle){btnManual.x, btnManual.y, srcMode.width, srcMode.height}))
        {
            chosenCar = 4; // red car
        }
        else if (CheckCollisionPointRec(mouse, (Rectangle){btnHardManual.x, btnHardManual.y, srcMode.width, srcMode.height}))
        {
            chosenCar = 5; // yellow car
        }
    }

    if (chosenCar == 0)
    {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(blackFrontTex,
                       (Rectangle){0, 0, blackFrontTex.width, blackFrontTex.height},
                       (Rectangle){65, 583, blackFrontTex.width * scale, blackFrontTex.height * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (chosenCar == 1)
    {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(blueFrontTex,
                       (Rectangle){0, 0, blueFrontTex.width, blueFrontTex.height},
                       (Rectangle){65, 583, blueFrontTex.width * scale, blueFrontTex.height * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (chosenCar == 2)
    {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(grayFrontTex,
                       (Rectangle){0, 0, grayFrontTex.width, grayFrontTex.height},
                       (Rectangle){65, 583, grayFrontTex.width * scale, grayFrontTex.height * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (chosenCar == 3)
    {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(pinkFrontTex,
                       (Rectangle){0, 0, pinkFrontTex.width, pinkFrontTex.height},
                       (Rectangle){65, 583, pinkFrontTex.width * scale, pinkFrontTex.height * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (chosenCar == 4)
    {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(redFrontTex,
                       (Rectangle){0, 0, redFrontTex.width, redFrontTex.height},
                       (Rectangle){65, 583, redFrontTex.width * scale, redFrontTex.height * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (chosenCar == 5)
    {
        DrawTextureRec(PC, srcMode, (Vector2){40, 580}, WHITE);
        DrawTexturePro(yellowFrontTex,
                       (Rectangle){0, 0, yellowFrontTex.width, yellowFrontTex.height},
                       (Rectangle){65, 583, yellowFrontTex.width * scale, yellowFrontTex.height * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
}

void update_car_position(float dt)
{
    float carSpeed = 150.0f; // num of pixel per second

    if (IsKeyDown(KEY_RIGHT))
    {
        carX += carSpeed * dt;
        carRotation = -90.0f;
    }
    if (IsKeyDown(KEY_LEFT))
    {
        carX -= carSpeed * dt;
        carRotation = 90.0f;
    }
    if (IsKeyDown(KEY_UP))
    {
        carY -= carSpeed * dt;
        carRotation = 0.0f;
    }
    if (IsKeyDown(KEY_DOWN))
    {
        carY += carSpeed * dt;
        carRotation = 180.0f;
    }

    if (carY < 0)
        carY = 130;
    if (carY > 550)
        carY = 480;
}

void place_car_at_start_pos()
{
    float scale = 0.75f;

    switch (chosenCar)
    {
    case 0: // black car
        DrawTexturePro(blackTopTex,
                       (Rectangle){0, 0, blackTopTex.width, blackTopTex.height},
                       (Rectangle){carX, carY, blackTopTex.width * 0.7, blackTopTex.height * 1.1},
                       (Vector2){blackTopTex.width / 2, blackTopTex.height / 2},
                       carRotation, WHITE);
        break;

    case 1: // blue car
        DrawTexturePro(blueTopTex,
                       (Rectangle){0, 0, blueTopTex.width, blueTopTex.height},
                       (Rectangle){carX, carY, blueTopTex.width * scale, blueTopTex.height * scale},
                       (Vector2){blueTopTex.width * scale / 2, blueTopTex.height * scale / 2},
                       carRotation, WHITE);
        break;

    case 2: // gray car
        DrawTexturePro(grayTopTex,
                       (Rectangle){0, 0, grayTopTex.width, grayTopTex.height},
                       (Rectangle){carX, carY, grayTopTex.width * scale, grayTopTex.height * 1.1},
                       (Vector2){grayTopTex.width / 2, grayTopTex.height / 2},
                       carRotation, WHITE);
        break;

    case 3: // pink car
        DrawTexturePro(pinkTopTex,
                       (Rectangle){0, 0, pinkTopTex.width, pinkTopTex.height},
                       (Rectangle){carX, carY, pinkTopTex.width * scale, pinkTopTex.height},
                       (Vector2){pinkTopTex.width * scale / 2, pinkTopTex.height * scale / 2},
                       carRotation, WHITE);
        break;

    case 4: // red car
        DrawTexturePro(redTopTex,
                       (Rectangle){0, 0, redTopTex.width, redTopTex.height},
                       (Rectangle){carX, carY, redTopTex.width * scale, redTopTex.height * scale},
                       (Vector2){redTopTex.width * scale / 2, redTopTex.height * scale / 2},
                       carRotation, WHITE);
        break;

    case 5: // yellow car
        DrawTexturePro(yellowTopTex,
                       (Rectangle){0, 0, yellowTopTex.width, yellowTopTex.height},
                       (Rectangle){carX, carY, yellowTopTex.width * scale, yellowTopTex.height * scale},
                       (Vector2){yellowTopTex.width * scale / 2, yellowTopTex.height * scale / 2},
                       carRotation, WHITE);
        break;
    }
}

void draw_buttons_direction(Texture2D PC)
{
    Rectangle srcUp = {129, 320, 60, 57};
    Rectangle srcDown = {192, 320, 60, 57};

    Rectangle srcLeft = {256, 320, 60, 57};
    Rectangle srcRight = {325, 320, 60, 57};

    Rectangle srcS = {64, 0, 60, 57};

    Vector2 posUp = {440, 660};
    Vector2 posDown = {440, 720};

    Vector2 posLeft = {posDown.x - 60, posDown.y};
    Vector2 posRight = {posDown.x + 65, posDown.y};

    Vector2 posS = {posDown.x - 150, posDown.y};

    DrawTextureRec(PC, srcUp, posUp, WHITE);
    DrawTextureRec(PC, srcDown, posDown, WHITE);
    DrawTextureRec(PC, srcLeft, posLeft, WHITE);
    DrawTextureRec(PC, srcRight, posRight, WHITE);

    DrawTextureRec(PC, srcS, posS, RED); // to imitate a 'stop' button
}

void drawFloorArrows(Texture2D PC, Rectangle srcArrow, Rectangle prev, Rectangle next, int floor)
{
    Vector2 origin = { srcArrow.width / 2.0f, srcArrow.height / 2.0f };

    bool canUp   = (floor < MAX_FLOOR);
    bool canDown = (floor > 0);

    Color upTint   = canUp   ? WHITE : Fade(GRAY, 0.5f);
    Color downTint = canDown ? WHITE : Fade(GRAY, 0.5f);

    DrawTexturePro(PC, srcArrow, prev, origin,  0.0f, upTint);
    DrawTexturePro(PC, srcArrow, next, origin, 180.0f, downTint);
}

void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[])
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Parking Simulator");
    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    int posX = (monitorWidth - SCREEN_WIDTH) / 2;
    int posY = (monitorHeight - SCREEN_HEIGHT) / 2;

    SetWindowPosition(posX, posY);
    SetTargetFPS(60);

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

    blackFrontTex = LoadTexture("Assets/black_car/blackFront.png");
    blackRightTex = LoadTexture("Assets/black_car/blackRight.png");
    blackTopTex = LoadTexture("Assets/black_car/blackTop.png");

    blueFrontTex = LoadTexture("Assets/blue_car/blueFront.png");
    blueRightTex = LoadTexture("Assets/blue_car/blueRight.png");
    blueTopTex = LoadTexture("Assets/blue_car/blueTop.png");

    grayFrontTex = LoadTexture("Assets/gray_car/grayFront.png");
    grayRightTex = LoadTexture("Assets/gray_car/grayRight.png");
    grayTopTex = LoadTexture("Assets/gray_car/grayTop.png");

    pinkFrontTex = LoadTexture("Assets/pink_car/pinkFront.png");
    pinkRightTex = LoadTexture("Assets/pink_car/pinkRight.png");
    pinkTopTex = LoadTexture("Assets/pink_car/pinkTop.png");

    redFrontTex = LoadTexture("Assets/red_car/redFront.png");
    redRightTex = LoadTexture("Assets/red_car/redRight.png");
    redTopTex = LoadTexture("Assets/red_car/redTop.png");

    yellowFrontTex = LoadTexture("Assets/yellow_car/yellowFront.png");
    yellowRightTex = LoadTexture("Assets/yellow_car/yellowRight.png");
    yellowTopTex = LoadTexture("Assets/yellow_car/yellowTop.png");

    current_floor = 0;

    Font font = LoadFontEx("resources/Minecraftia.ttf", 15, NULL, 0);

    const char *message1 = "Welcome to our Parking Simulator";
    const char *message2 = "Choose a Game Mode :";

    int letters1 = 0;
    float timer1 = 0.0f;

    int letters2 = 0;
    float letterTimer2 = 0.0f;

    float letterDelay = 0.05f;

    Screen currentScreen = SCREEN_ORDORED_PANEL;

    destPreviewLevel = (Rectangle){750, 750, srcArrow.width, srcArrow.height};
    destNextLevel = (Rectangle){50, 750, srcArrow.width, srcArrow.height};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(background, 0, 0, WHITE);

        panel();
        if (current_floor == 0)
        {
            draw_entrance_barrier();
            draw_exit_barrier();
        }
        draw_parking_places(num_parking_places, places);

        // permanent arrows (independently of the mode)
        Rectangle srcArrow = {129, 64, 60, 57};

        drawFloorArrows(PC, srcArrow, destPreviewLevel, destNextLevel, current_floor);

        Rectangle srcReturn = {0, 130, 60, 60};
        Rectangle destReturn = {140, 750, srcArrow.width, srcArrow.height};
        DrawTexturePro(PC, srcReturn, destReturn, origin1, 0, BLUE);

        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();
        DrawText(TextFormat("(x;y) = (%d;%d)", (int)mouse.x, (int)mouse.y), 10, 10, 12, BLACK);
        //to change floors
         if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(mouse, destPreviewLevel))
                {
                    if (current_floor < MAX_FLOOR)
                    {
                        current_floor++;
                        reload_floor(current_floor, places, &num_parking_places);
                    }
                    else
                    {
                        printf("[FLOOR] Already on the top floor (2)\n");
                    }
                }
                else if (CheckCollisionPointRec(mouse, destNextLevel))
                {
                    if (current_floor > 0)
                    {
                        current_floor--;
                        reload_floor(current_floor, places, &num_parking_places);
                    }
                    else
                    {
                        printf("[FLOOR] Already on the ground floor (0)\n");
                    }
                }
            }
            update_barrier_angles();
            handle_stations_input();
            handle_automatic_opening();

        switch (currentScreen)
        {
        case SCREEN_ORDORED_PANEL:
            if (letters1 < strlen(message1))
            {
                timer1 += dt;
                if (timer1 >= letterDelay)
                {
                    letters1++;
                    timer1 = 0.0f;
                }
            }
            else
            {
                if (letters2 < strlen(message2))
                {
                    letterTimer2 += dt;
                    if (letterTimer2 >= letterDelay)
                    {
                        letters2++;
                        letterTimer2 = 0.0f;
                    }
                }
            }

            DrawTextEx(font, TextSubtext(message1, 0, letters1), (Vector2){200, 590}, 24, 2, WHITE);
            DrawTextEx(font, TextSubtext(message2, 0, letters2), (Vector2){280, 620}, 22, 2, WHITE);

            init_ordored_panel_menu();
            ordored_panel_menu(font);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(mouse, btnRandom))
                    currentScreen = SCREEN_RANDOM;
                if (CheckCollisionPointRec(mouse, btnManual))
                    currentScreen = SCREEN_MANUAL;
                if (CheckCollisionPointRec(mouse, btnHardManual))
                    currentScreen = SCREEN_HARD_MANUAL;
            }
            break;

        case SCREEN_RANDOM:
            DrawText("Random mode", 200, 400, 20, DARKBLUE);
            if (IsKeyPressed(KEY_ESCAPE) ||
                (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, btnReturn)))
            {
                currentScreen = SCREEN_ORDORED_PANEL;
            }
            break;

        case SCREEN_MANUAL:
            choose_your_car(font);
            place_car_at_start_pos();

            if (chosenCar != -1)
            {
                Rectangle srcReturn1 = {0, 130, -60, 60};
                Rectangle destNextStep = {625, 725, srcArrow.width, srcArrow.height};
                DrawTexturePro(PC, srcReturn1, destNextStep, origin, 0, GREEN);
                DrawRectangleLines((int)destNextStep.x, (int)destNextStep.y, (int)destNextStep.width, (int)destNextStep.height, RED);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, destNextStep))
                {
                    currentScreen = SCREEN_DIRECTION;
                }
            }

            if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, btnReturn)))
            {
                currentScreen = SCREEN_ORDORED_PANEL;
            }
            break;

        case SCREEN_DIRECTION:
            draw_buttons_direction(PC);
            update_car_position(dt);
            place_car_at_start_pos();

            if (IsKeyPressed(KEY_ESCAPE) || (CheckCollisionPointRec(mouse, btnReturn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
            {
                currentScreen = SCREEN_MANUAL;
            }
            break;

        case SCREEN_HARD_MANUAL:
            DrawText("Hard Manual mode", 200, 400, 20, DARKGREEN);
            if (IsKeyPressed(KEY_ESCAPE) ||
                (CheckCollisionPointRec(mouse, btnReturn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
            {
                currentScreen = SCREEN_ORDORED_PANEL;
            }
            break;
        }

        EndDrawing();
    }

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
    UnloadFont(font);

    UnloadTexture(blackFrontTex);
    UnloadTexture(blackRightTex);
    UnloadTexture(blackTopTex);

    UnloadTexture(blueFrontTex);
    UnloadTexture(blueRightTex);
    UnloadTexture(blueTopTex);

    UnloadTexture(grayFrontTex);
    UnloadTexture(grayRightTex);
    UnloadTexture(grayTopTex);

    UnloadTexture(pinkFrontTex);
    UnloadTexture(pinkRightTex);
    UnloadTexture(pinkTopTex);

    UnloadTexture(redFrontTex);
    UnloadTexture(redRightTex);
    UnloadTexture(redTopTex);

    UnloadTexture(yellowFrontTex);
    UnloadTexture(yellowRightTex);
    UnloadTexture(yellowTopTex);

    CloseWindow();
}
