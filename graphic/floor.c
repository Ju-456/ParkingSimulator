#include "window.h"

static const char *FLOOR_FILES[3] = {"graph_floor_0.json", "graph_floor_1.json", "graph_floor_2.json"};

void request_floor_change(int direction, Parking places[], int *num_parking_places) {
    int oldFloor = currentFloor;
    int newFloor = currentFloor + direction;

    if (newFloor < 0 || newFloor > MAX_FLOOR)
        return;

    // Award 10 seconds only when moving up from 0->1 or 1->2
    if (direction == 1 && (oldFloor == 0 || oldFloor == 1)) {
        if (timerActive && timerStartTime >= 0.0) {
            // Add 10s to remaining time by increasing timerDuration
            timerDuration += 10.0;
        }
    }

    currentFloor = newFloor;
    carFloor = newFloor;
    reload_floor(currentFloor, places, num_parking_places);
}

// reload a floor to reset the states
void reload_floor(int floor, Parking places[], int *num_parking_places) {
    char path[PATH_MAX] = {0};
    build_path(path, "graph_json/", FLOOR_FILES[floor]);

    *num_parking_places = count_number_places(path);
    if (load_graph_from_json(path, *num_parking_places, places)) {
        init_direction_parking_places(*num_parking_places, places);
    } else {
        fprintf(stderr, "[FLOOR] Error loading: %s\n", FLOOR_FILES[floor]);
    }

    entranceState = exitState = 0;
    entranceTargetAngle = exitTargetAngle = 0.0f;
    entranceAngle = exitAngle = 0.0f;
    entranceTriggerTime = exitTriggerTime = -1.0;
    entranceOpenTime = exitOpenTime = -1.0;
}

void draw_floor() {
    // top
    if (currentFloor < 2) {
        DrawTexture(floor_exit, 780.0f, 20.0f, WHITE);
    }
    // bottom
    if (currentFloor > 0) {
        DrawTexture(floor_exit, 10.0f, 460.0f, WHITE);
    }

    DrawTexture(floor_indicator[currentFloor], 740.0f, 20.0f, WHITE);
}

void delimitation_of_playground() {
    Rectangle carRect = {carX - 20, carY - 20, 40, 40};
    Rectangle entranceRect = {60, 73, 40, 27};
    Rectangle exitRect = {630, 455, 40, 80};
    Rectangle exitEndRect = {730, 455, 40, 80};

    if (currentFloor == 0 && payment == 1 && exit_is_passable()) {
        Rectangle carRect = {carX - 20, carY - 20, 40, 40};
        if (CheckCollisionRecs(carRect, exitEndRect)) {
            gameFinished = true;
        }
    }

    if (currentFloor == 0) {
        if (CheckCollisionRecs(carRect, entranceRect) && ticket == 0) {
            carX = 70;
            carY = 73;
        }
        if (!insideParking && carX - 20 > entranceRect.x + entranceRect.width) {
            insideParking = true;
        }

        if (insideParking && CheckCollisionRecs(carRect, entranceRect)) {
            carX = entranceRect.x + entranceRect.width + 80;
        }
        if (CheckCollisionRecs(carRect, exitRect) && !exit_is_passable()) {
            carX = 600;
            carY = 500;
        }
    }
    if ((carY > 460 && carY < 540) && (carX > 0 && carX < 20)) {
        if (currentFloor > 0) {
            floorChangeRequestedDown = true;
            carFloor--;
            carX = 710;
            carY = 75;
        }
    }
    if (carY < 0)
        carY = 130; // because height is ± 180 so we're often used 130 for all boundary adjustments
    if (carY > 550)
        carY = 480;
    if (currentFloor == 0) {                                          // barriers wall are managed only on floor P-0
        if ((carY > 100 && carY < 110) && (carX > 0 && carX < 150)) { // First barrier wall
            carY = 100;
        } else if ((carY > 440 && carY < 450) && (carX > 660 && carX < 750)) { // Second barrier wall
            carY = 400;
        }
    }
    if (carX > 750) {                 // Right boundary
        if (carY > 0 && carY < 100) { // chgmt of level IF != P-2
            if (currentFloor < MAX_FLOOR) {
                floorChangeRequestedUp = true;
                carFloor++;
                carX = 20;
                carY = 75;
            } else {
                carX = 670;
            }
        } else if (carY >= 100 && carY <= 435) { // 100 < carY < 435
            carX = 670;
        } else if (currentFloor != 0) {
            carX = 670;
        }
    } else if (carX < 0) { // Left boundary
        if (carY > 0 && carY < 100) {
            carY = 400;
        } else {
            carX = 130;
        }
    }
}