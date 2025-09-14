#include "window_menu.h"

int entrance_state = 0; // 0 - closed, 1 - opened
int exit_state = 0;
int barrier_type = 0;   // 0 - entrance, 1 - exit

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
static const char* FLOOR_FILES[3] = {
    "graph_floor_0.json", "graph_floor_1.json", "graph_floor_2.json"
};

// void full_screen_mode(int num_parking_places, Parking places[], Parking scaled_places[])
// {
//     int screen_width = GetScreenWidth();
//     int screen_height = GetScreenHeight();

//     float scale = fminf((float)screen_width / SCREEN_WIDTH, (float)screen_height / SCREEN_HEIGHT);
//     float offsetX = (screen_width - SCREEN_WIDTH * scale) / 2.0f;
//     float offsetY = (screen_height - SCREEN_HEIGHT * scale) / 2.0f;

//     for (int i = 0; i < num_parking_places; i++)
//     {
//         scaled_places[i] = places[i];
//         scaled_places[i].x = places[i].x * scale + offsetX;
//         scaled_places[i].y = places[i].y * scale + offsetY;
//     }
// }

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
    if (current_floor != 0) {
        if (IsKeyPressed(KEY_T) || IsKeyPressed(KEY_P)) {
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
            printf("[EXIT] Ticket payed.\n");
        }
        else
        {
            printf("[EXIT] Ticket was already payed.\n");
        }
    }
}

void handle_automatic_opening()
{
    if (current_floor != 0) return; 
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
        ticket=0;
        payment=0;
        exit_open_time = -1.0;
    }
}

void draw_entrance_barrier()
{
    float x = 140.0f, y = 20.0f;

    DrawTexture(barrier_wall, 10.0f, 103.0f, WHITE);
    DrawTexture(entrance_ticket_dispenser, 100.0f, 20.0f, WHITE);

    // axis to turn
    Vector2 origin = (Vector2){0.0f, 0.0f};

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
void reload_floor(int floor, Parking places[], int* num_parking_places)
{
    char path[PATH_MAX] = {0};
    build_path(path, "graph_json/", FLOOR_FILES[floor]);

    *num_parking_places = count_number_places(path);
    if (load_graph_from_json(path, *num_parking_places, places)) {
        init_direction_parking_places(*num_parking_places, places);
    } else {
        fprintf(stderr, "[FLOOR] Error loading: %s\n", FLOOR_FILES[floor]);
    }

    entrance_state = exit_state = 0;
    entrance_target_angle = exit_target_angle = 0.0f;
    entrance_angle = exit_angle = 0.0f;
    entrance_trigger_time = exit_trigger_time = -1.0;
    entrance_open_time = exit_open_time = -1.0;
}

void handle_floor_input(Parking places[], int* num_parking_places)
{
    // UP
    if (IsKeyPressed(KEY_U)) {
        if (current_floor < 2) {
            current_floor++;
            reload_floor(current_floor, places, num_parking_places);
        } else {
            printf("[FLOOR] Already on the top floor (2)\n");
        }
    }

    // DOWN
    if (IsKeyPressed(KEY_D)) {
        if (current_floor > 0) {
            current_floor--;
            reload_floor(current_floor, places, num_parking_places);
        } else {
            printf("[FLOOR] Already on the ground floor (0)\n");
        }
    }
}

void draw_floor()
{
    DrawTexture(floor_exit, 780.0f, 20.0f, WHITE);
    DrawTexture(floor_indicator[current_floor], 740.0f, 20.0f, WHITE);
}


void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[])
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Parking Simulator");
    SetTargetFPS(60);

    background = LoadTexture("Assets/background.png");
    parking_place = LoadTexture("Assets/parking_place.png");
    panel_menu = LoadTexture("Assets/panel_menu.png");
    entrance_barrier = LoadTexture("Assets/entrance_barrier.png");
    exit_barrier = LoadTexture("Assets/exit_barrier.png");
    barrier_wall = LoadTexture("Assets/barrier_wall.png");
    entrance_ticket_dispenser = LoadTexture("Assets/entrance_ticket_dispenser.png");
    exit_pay_station = LoadTexture("Assets/exit_pay_station.png");
    floor_exit            = LoadTexture("Assets/floor_exit.png");
    floor_indicator[0]    = LoadTexture("Assets/floor_indicator0.png");
    floor_indicator[1]    = LoadTexture("Assets/floor_indicator1.png");
    floor_indicator[2]    = LoadTexture("Assets/floor_indicator2.png");

    current_floor = 0;

    while (!WindowShouldClose())
    {

        handle_stations_input();
        handle_floor_input(places, &num_parking_places); 
        handle_automatic_opening();

        // animation of angles
        update_barrier_angles();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);

        panel();
        if (current_floor == 0) {
            draw_entrance_barrier();
            draw_exit_barrier();
        }
        
        draw_parking_places(num_parking_places, places);
        draw_floor(); 

        EndDrawing();
    }

    UnloadTexture(background);
    UnloadTexture(parking_place);
    UnloadTexture(panel_menu);
    UnloadTexture(entrance_barrier);
    UnloadTexture(exit_barrier);
    UnloadTexture(barrier_wall);
    UnloadTexture(exit_pay_station);
    UnloadTexture(entrance_ticket_dispenser);
    UnloadTexture(floor_exit);
    UnloadTexture(floor_indicator[0]);
    UnloadTexture(floor_indicator[1]);
    UnloadTexture(floor_indicator[2]);

    CloseWindow();
}