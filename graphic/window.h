#ifndef WINDOW_H
#define WINDOW_H

#include "parking.h"
#include "texture.h"

Texture2D background;

void init_window(const char *full_path_json, int num_parking_places, Parking places[]);

#endif