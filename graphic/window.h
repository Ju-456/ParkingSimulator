#ifndef WINDOW_H
#define WINDOW_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#include <math.h>
#include "parking.h"
#include "texture.h"

Texture2D background;
Texture2D parkingPlace;

void full_screen_mode(int num_parking_places, Parking places[], Parking scaled_places[]);
void draw_parking_places(int n, Parking places[]);
void init_window(const char *full_path_json, int num_parking_places, Parking places[], Parking scaled_places[]);

#endif