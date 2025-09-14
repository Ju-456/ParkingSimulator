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
Texture2D exit_barrier;
Texture2D barrier_wall;
Texture2D exit_pay_station;
Texture2D entrance_ticket_dispenser;

Texture2D floor_exit;
Texture2D floor_indicator[3];

// window_menu.c functions
//void full_screen_mode(int num_parking_places, Parking places[], Parking scaled_places[]);
void draw_parking_places(int n, Parking places[]);
void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[]);

void draw_exit_barrier();
void draw_entrance_barrier();
void handle_stations_input();
void handle_automatic_opening();
void barrier_management(int barrier_type, int barrier_state);
void update_barrier_angles();

void draw_floor();
void handle_floor_input(Parking places[], int* num_parking_places);
void reload_floor(int floor, Parking places[], int* num_parking_places);


// menu.c functions
void panel();

#endif