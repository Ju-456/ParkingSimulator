#ifndef PARKING_H
#define PARKING_H

typedef struct {
    char id[2];  
    int x;
    int y;              
} Parking;

Texture2D parkingPlace;
Texture2D background;

void load_parking_texture();

#endif