#include "window.h"

static int simIndex = 0;

void request_simulation_start(int sim) {
    simIndex = sim;
    simPendingStart = true;
    simRunning = false;
    entranceTriggerTime = GetTime();
    printf("request_simulation_start = sim=%d; entranceTriggerTime=%.3f\n", sim, entranceTriggerTime);
}

void select_random_car() {
    chosenCar = rand() % 6;
    printf("chosenCar = %d\n", chosenCar);
}

void first_simulation(float dt) {
    static float simTime = 0.0f;
    if (!simRunning || simIndex != 0)
        return;

    simTime += dt;

    if (simTime < 11.0f) {
        carX += 200.0f * dt;
        carY = 300.0f * dt; // doesn't work in others way for the moment bc of barriers maybe
        carRotation = -90.0f;
    } else if (simTime < 12.0f) {
        carRotation += 90.0f * dt;
    } else if (simTime < 13.0f) {
        carY += 227.0f * dt;
    } else if (simTime < 13.5f) {
        carRotation += 180.0f * dt;
    } else if (simTime < 13.7f) {
        carX += 200.0f * dt;
    } else if (simTime < 14.2f) {
        carX += -250.0f * dt;
    }

    // printf("simTime = %.2f\n", simTime);
    debug_print_state_periodic();
    delimitation_of_playground();
}

void second_simulation(float dt) {
    if (!simRunning || simIndex != 1)
        return;

    // programm a new simulation here (soon)
    delimitation_of_playground();
}

void third_simulation(float dt) {
    if (!simRunning || simIndex != 2)
        return;

    // programm a new simulation here (soon)
    delimitation_of_playground();
}

// Per-frame update
void update_simulation(float dt) {
    if (!simRunning && simPendingStart) {
        simRunning = true;
        simPendingStart = false;
    }

    switch (simIndex) {
    case 0:
        first_simulation(dt);
        break;
    case 1:
        second_simulation(dt);
        break;
    case 2:
        third_simulation(dt);
        break;
    default:
        break;
    }
}

void debug_print_state_periodic() {
    static double lastPrint = 0.0;
    double now = GetTime();
    const double interval = 0.5; // print every 0.5 seconds

    if ((now - lastPrint) < interval)
        return;
    lastPrint = now;

    printf("Car X: %.2f, Car Y: %.2f, Rotation: %.2f, Floor: %d\n", carX, carY, carRotation, carFloor);
}