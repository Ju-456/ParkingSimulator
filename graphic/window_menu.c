#include "window_menu.h"

int entrance_state = 0; // 0 - closed, 1 - opened
int exit_state = 0; // 0 - closed, 1 - opened
int barrier_type = 0; // 0 - entrance, 1 - exit

float entrance_angle = 0.0f; 
float exit_angle = 0.0f; 
float entrance_target_angle = 0.0f; 
float exit_target_angle = 0.0f; 

const float BARRIER_SPEED = 180.0f; // deg/s

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
    const float scale = 0.5f; // 50% of the real size

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

void panel(){
    DrawTexture(panel_menu, 0, 560, WHITE);
}

void update_barrier_angles(){
    float delta_time = GetFrameTime(); // elapsed time since the last frame (in seconds)s

    //entrance
    float deltaE = entrance_target_angle - entrance_angle; // difference between the desired angle and the current angle
    float stepE = BARRIER_SPEED * delta_time; // how much we can rotate this frame

    if (fabsf(deltaE) <= stepE) {
        entrance_angle = entrance_target_angle; // we can reach the target this frame
    } else {
        entrance_angle += (deltaE > 0 ? stepE : -stepE); // move towards the target
    }

    //exit
    float deltaX = exit_target_angle - exit_angle;
    float stepX = BARRIER_SPEED * delta_time;

    if (fabsf(deltaX) <= stepX) {
        exit_angle = exit_target_angle;
    } else {
        exit_angle += (deltaX > 0 ? stepX : -stepX);
    }
    
}
void barrier_management(int barrier_type, int barrier_state){
    if (barrier_type == 0) //entrance
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
       }else{
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
            exit_target_angle = -90.0f;
       }else{
        fprintf(stderr, "Wrong barrier state: %d\n", barrier_state);
       }
    }
    else
    {
        fprintf(stderr, "Wrong barrier type: %d\n", barrier_type);
    }
    

}
void handle_barrier_input(){
    if (IsKeyPressed(KEY_E)) //entrance
    {
        barrier_management(0, entrance_state ? 0 : 1);
    }
    if (IsKeyPressed(KEY_X)) //exit
    {
        barrier_management(1, exit_state ? 0 : 1);
    }
    
}

void draw_entrance_barrier(){
    float x = 140.0f, y = 10.0f;
    
    DrawTexture(barrier_wall, 10, 93.0f, WHITE);


    Rectangle src = (Rectangle){0, 0, (float)entrance_barrier.width, (float)entrance_barrier.height};
    Rectangle dst = (Rectangle){x, y, (float)entrance_barrier.width, (float)entrance_barrier.height};

    // axis to turn
    Vector2 origin = (Vector2){0.0f, 0.0f};

    DrawTexturePro(entrance_barrier, src, dst, origin, entrance_angle, WHITE);


}
void draw_exit_barrier(){
    DrawTexture(barrier_wall, 660, 465, WHITE);

    float x = 645.0f, y = 475.0f;

    Rectangle src = (Rectangle){0, 0, (float)exit_barrier.width, (float)exit_barrier.height};

    Vector2 origin = {0.0f, (float)exit_barrier.height};
    Rectangle dst = (Rectangle){x + origin.x, y + origin.y, (float)exit_barrier.width, (float)exit_barrier.height};

    DrawTexturePro(exit_barrier, src, dst, origin, exit_angle, WHITE);
    
}

void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[], Parking scaled_places[]) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Parking Simulator");
    SetTargetFPS(60);

    background = LoadTexture("Assets/background.png");
    parking_place = LoadTexture("Assets/parking_place.png");
    panel_menu = LoadTexture("Assets/panel_menu.png");
    entrance_barrier = LoadTexture("Assets/entrance_barrier.png");
    exit_barrier = LoadTexture("Assets/exit_barrier.png");
    barrier_wall = LoadTexture("Assets/barrier_wall.png");
    //entrance_pay_station = LoadTexture("Assets/entrance_pay_station.png");
    //exit_ticket_dispenser = LoadTexture("Assets/exit_ticket_dispenser.png");

    while (!WindowShouldClose()) {

        // open/close barriers management
        handle_barrier_input();

        // animation of angles
        update_barrier_angles();

        // result
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);

        panel();
        draw_entrance_barrier();
        draw_exit_barrier();
        full_screen_mode(num_parking_places, places, scaled_places);
        draw_parking_places(num_parking_places, scaled_places);

        EndDrawing();
    }

    UnloadTexture(background);
    UnloadTexture(parking_place);
    UnloadTexture(panel_menu);
    UnloadTexture(entrance_barrier);
    UnloadTexture(exit_barrier);
    UnloadTexture(barrier_wall);
    
    CloseWindow();
}