#ifndef PARKING_H
#define PARKING_H

#include "parson.h"
#include "texture.h"

typedef struct {
    char id[2];  
    int x;
    int y;              
} Parking;

int count_number_places(char full_path_json[]);
void read_id_from_json(const char *full_path_json, Parking places[], int num_parking_places);
int load_graph_from_json(const char *full_path_json, int num_parking_places, Parking places[]);

#endif