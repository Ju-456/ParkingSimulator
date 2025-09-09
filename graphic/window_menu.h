#ifndef WINDOW_MENU_H
#define WINDOW_MENU_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#include <math.h>
#include "parking.h"

Texture2D background;
Texture2D parking_place;
Texture2D pannel_menu;
Texture2D pannel_menu;

Texture2D entrance_barrier;
Texture2D exit_barrier;
Texture2D barrier_wall;
//Texture2D entrance_pay_station;
//Texture2D exit_ticket_dispenser;

// window.c functions
void full_screen_mode(int num_parking_places, Parking places[], Parking scaled_places[]);
void draw_parking_places(int n, Parking places[]);
void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[], Parking scaled_places[]);

// menu.c functions
void pannel();

#endif