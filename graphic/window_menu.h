#ifndef WINDOW_MENU_H
#define WINDOW_MENU_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#include <math.h>
#include "parking.h"

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

typedef enum {
    SCREEN_ORDORED_PANEL,
    SCREEN_RANDOM,
    SCREEN_MANUAL,
    SCREEN_HARD_MANUAL
} Screen;

void draw_parking_places(int n, Parking places[]);
void panel();
void update_barrier_angles();
void barrier_management(int barrier_type, int barrier_state);

void handle_stations_input();
void handle_automatic_opening();

void draw_entrance_barrier();
void draw_exit_barrier();

void reload_floor(int floor, Parking places[], int* num_parking_places);
void handle_floor_input(Parking places[], int* num_parking_places);
void draw_floor();

void init_ordored_panel_menu();
void ordored_panel_menu(Font font);
void choose_your_car(Font font);
void choose_your_car_condition();
void place_car_at_start_pos();
void draw_buttons_direction(Texture2D PC);

void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[]);

#endif