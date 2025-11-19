#include "window.h"

void update_barrier_angles() {
    float delta_time = GetFrameTime(); // elapsed time since the last frame (in seconds)s

    // entrance
    float deltaE = entranceTargetAngle - entranceAngle; // difference between the desired angle and the current angle
    float stepE = BARRIER_SPEED * delta_time;           // how much we can rotate this frame

    if (fabsf(deltaE) <= stepE) {
        entranceAngle = entranceTargetAngle; // we can reach the target this frame
    } else {
        entranceAngle += (deltaE > 0 ? stepE : -stepE); // move towards the target
    }

    // exit
    float deltaX = exitTargetAngle - exitAngle;
    float stepX = BARRIER_SPEED * delta_time;

    if (fabsf(deltaX) <= stepX) {
        exitAngle = exitTargetAngle;
    } else {
        exitAngle += (deltaX > 0 ? stepX : -stepX);
    }
}

void barrier_management(int barrierType, int barrier_state) {
    if (barrierType == 0) { // entrance

        if (barrier_state == 0) { // we want to close it
            entranceState = 0;
            entranceTargetAngle = 0.0f;
        } else if (barrier_state == 1) { // we want to open it
            entranceState = 1;
            entranceTargetAngle = -90.0f;
        }
    } else if (barrierType == 1) { // exit
        if (barrier_state == 0) {  // we want to close it
            exitState = 0;
            exitTargetAngle = 0.0f;
        } else if (barrier_state == 1) { // we want to open it
            exitState = 1;
            exitTargetAngle = 90.0f;
        }
    }
}

void handle_automatic_opening() {
    double now = GetTime();

    // Only handle entrance/exit on floor 0 (manual and hard mode)
    if (currentFloor == 0) {
        // entrance
        // open after taking a ticket
        if (entranceTriggerTime >= 0.0 && (now - entranceTriggerTime) >= OPEN_DELAY) {
            barrier_management(0, 1);   // open
            entranceTriggerTime = -1.0; // no pending events
            entranceOpenTime = now;
        }

        // close after 5sec
        if (entranceOpenTime >= 0.0 && (now - entranceOpenTime) >= CLOSE_DELAY) {
            barrier_management(0, 0); // close
            entranceOpenTime = -1.0;
        }

        // exit
        // open after payement
        if (exitTriggerTime >= 0.0 && (now - exitTriggerTime) >= OPEN_DELAY) {
            barrier_management(1, 1);
            exitTriggerTime = -1.0;
            exitOpenTime = now;
        }

        // close after 5sec
        if (exitOpenTime >= 0.0 && (now - exitOpenTime) >= CLOSE_DELAY) {
            barrier_management(1, 0);
            ticket = 0;
            payment = 0;
            exitOpenTime = -1.0;
        }
    } else {
        // In random/simulation mode on upper floors, prepare exit barrier
        // Open exit barrier when simulation is near completion or car is exiting
        if (simRunning && exitTriggerTime < 0.0) {
            // Exit barrier should be open during random simulation
            barrier_management(1, 1);
            exitTriggerTime = now;
            exitOpenTime = now;
        }
    }
}

void draw_entrance_barrier() {
    float x = 140.0f, y = 20.0f;

    DrawTexture(barrier_wall, 10.0f, 103.0f, WHITE);
    DrawTexture(entrance_ticket_dispenser, 100.0f, 20.0f, WHITE);

    Rectangle src = (Rectangle){0, 0, (float)entrance_barrier.width, (float)entrance_barrier.height};
    Rectangle dst = (Rectangle){x, y, (float)entrance_barrier.width, (float)entrance_barrier.height};

    DrawTexturePro(entrance_barrier, src, dst, origin, entranceAngle, WHITE);
}

void draw_exit_barrier() {
    float x = 645.0f, y = 455.0f;

    DrawTexture(barrier_wall, 660.0f, 445.0f, WHITE);
    DrawTexture(exit_pay_station, 610.0f, 508.0f, WHITE);

    Rectangle src = (Rectangle){0, 0, (float)exit_barrier.width, (float)exit_barrier.height};

    Vector2 origin = {0.0f, (float)exit_barrier.height};
    Rectangle dst = (Rectangle){x + origin.x, y + origin.y, (float)exit_barrier.width, (float)exit_barrier.height};

    DrawTexturePro(exit_barrier, src, dst, origin, exitAngle, WHITE);
}

bool exit_is_passable() {
    return (payment == 1) || (exitAngle >= 85.0f);
}