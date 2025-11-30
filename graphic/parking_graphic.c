#include "window.h"

void draw_parking_places(int n, Parking places[]){
    const float width = 180.0f;
    const float height = 95.0f;
    const float scale = 0.62f; // 62% of the real size

    for (int i = 0; i < n; i++)
    {
        float w = width * scale;
        float h = height * scale;

        Rectangle src = (Rectangle){0, 0, width, height};
        Rectangle dest = (Rectangle){places[i].x, places[i].y, w, h};
        Texture2D texture = places[i].occupied ? busy_parking_place : free_parking_place;

        if (places[i].direction == 0)
        { // positive direction
            DrawTexturePro(texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else
        { // negative direction => Renversed picture left/right
            Rectangle srcMir = (Rectangle){width, 0, -width, height};
            DrawTexturePro(texture, srcMir, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}

void draw_parked_cars(int n, Parking places[]){
    for (int i = 0; i < n; i++)
    {
        if (places[i].occupied)
        {
            if (i == parkedPlaceIndex) 
                continue;
            int colorIndex = places[i].colorIndex;
            float scale = 0.8f;

            if (colorIndex == 5)
            {
                scale = 0.7f;
            }
            if (colorIndex == 4)
            {
                scale = 0.9f;
            }
            if (colorIndex == 1)
            {
                scale = 0.7f;
            }
            float rotation;
            if (places[i].direction == 0)
            {
                rotation = 90.0f;
                DrawTexturePro(carOrient[colorIndex].top,
                               (Rectangle){0, 0, carOrient[colorIndex].top.width,
                                           carOrient[colorIndex].top.height},
                               (Rectangle){places[i].x + 70, places[i].y + 20,
                                           carOrient[colorIndex].top.width * scale,
                                           carOrient[colorIndex].top.height * scale},
                               (Vector2){carOrient[colorIndex].top.width * 0.25f,
                                         carOrient[colorIndex].top.height * 0.25f},
                               rotation, WHITE);
            }
            else
            {
                rotation = -90.0f;
                DrawTexturePro(carOrient[colorIndex].top,
                               (Rectangle){0, 0, carOrient[colorIndex].top.width,
                                           carOrient[colorIndex].top.height},
                               (Rectangle){places[i].x + 50, places[i].y + 40,
                                           carOrient[colorIndex].top.width * scale,
                                           carOrient[colorIndex].top.height * scale},
                               (Vector2){carOrient[colorIndex].top.width * 0.25f,
                                         carOrient[colorIndex].top.height * 0.25f},
                               rotation, WHITE);
            }
        }
    }
}

void init_parking_state(int n, Parking places[]){
    for (int i = 0; i < n; i++)
    {
        places[i].occupied = (GetRandomValue(0, 100) < 50);
        if (places[i].occupied)
        {
            places[i].colorIndex = GetRandomValue(0, NUM_CARS - 1);
        }
    }
}

void reset_parking_state(Parking places[], int *num_parking_places){
    currentFloor = 0;
    carFloor = 0;

    entranceState = 0;
    exitState = 0;
    entranceAngle = 0.0f;
    exitAngle = 0.0f;
    entranceTargetAngle = 0.0f;
    exitTargetAngle = 0.0f;

    entranceTriggerTime = -1.0;
    exitTriggerTime = -1.0;
    entranceOpenTime = -1.0;
    exitOpenTime = -1.0;

    ticket = 0;
    payment = 0;
    insideParking = false;
    gameFinished = false;

    reload_floor(currentFloor, places, num_parking_places);

    carX = 70;
    carY = 73;
    carRotation = -90.0f;
}

bool check_collision_with_parking(float carX, float carY, Parking places[], int n){
    Rectangle carRect = {carX - 20, carY - 20, 40, 40};

    for (int i = 0; i < n; i++)
    {
        if (places[i].occupied)
        {
            // parking place sizes
            float w = 180.0f * 0.62f;
            float h = 95.0f * 0.62f;
            Rectangle placeRect = {places[i].x, places[i].y, w, h};

            if (CheckCollisionRecs(carRect, placeRect))
            {
                return true;
            }
        }
    }
    return false;
}