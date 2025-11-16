#include "window.h"

static int simIndex = 0;
static float simTime = 0.0f;
static int simFileIndex = -1;
static char simFilename[64];
static FILE *simFile = NULL;
static FILE *replay_fp = NULL;
static char replay_fullpath[PATH_MAX];
static int replay_active = 0;
static int lastLoadedCar = -1;
#define SIMDATA_PATH "graphic/simdata/"

// to create a scenario for a random simu
void open_new_sim_file_session() {
    if (simFile)
        return;

    simFileIndex = 0;
    char fullpath[PATH_MAX];

    while (1) {
        snprintf(simFilename, sizeof(simFilename), "simulation_data_%d.txt", simFileIndex);

        build_path(fullpath, SIMDATA_PATH, simFilename);

        FILE *test = fopen(fullpath, "r");
        if (!test)
            break;
        fclose(test);

        simFileIndex++;
    }

    build_path(fullpath, SIMDATA_PATH, simFilename);
    simFile = fopen(fullpath, "a");

    printf("New session file : %s\n", fullpath);
}

void close_sim_file_session() {
    if (simFile) {
        fclose(simFile);
        simFile = NULL;
        printf("Session closed ! %s\n", simFilename);
    }
}

// when you choose manual mode, we save car's mvt to replay it later as a rand simu
void collect_and_save_simulation_data_auto(float dt) {
    if (!simFile)
        open_new_sim_file_session();

    simTime += dt;

    fprintf(simFile, "Car X: %.2f, Car Y: %.2f, Rotation: %.2f, Floor: %d, simTime : %.2f\n", carX, carY, carRotation, carFloor, simTime);

    fflush(simFile);
}

// to start a random simu
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

// to apply created  scenarios for a random simu
static void open_random_sim_file(int chosenSim) {
    if (replay_fp && lastLoadedCar == chosenSim)
        return;

    if (replay_fp) {
        fclose(replay_fp);
        replay_fp = NULL;
    }

    lastLoadedCar = chosenSim;

    snprintf(replay_fullpath, sizeof(replay_fullpath), "graphic/simdata/simulation_data_%d.txt", chosenSim);

    replay_fp = fopen(replay_fullpath, "r");

    if (!replay_fp) {
        printf("ERROOOR, can't open : %s\n", replay_fullpath);
        replay_active = 0;
        return;
    }

    replay_active = 1;
    printf("Random sim opened : %s\n", replay_fullpath);
}

static int random_sim_read_and_apply() {
    if (!replay_active || !replay_fp)
        return 0;

    char line[512];

    if (!fgets(line, sizeof(line), replay_fp)) {
        fclose(replay_fp);
        replay_fp = NULL;
        replay_active = 0;
        return 0;
    }

    float rx, ry, rrot;
    int rfloor;
    double rtime;

    int scanned = sscanf(line, "Car X: %f, Car Y: %f, Rotation: %f, Floor: %d, simTime : %lf", &rx, &ry, &rrot, &rfloor, &rtime);

    if (scanned < 4) {
        scanned = sscanf(line, "Car X:%f ,Car Y:%f ,Rotation:%f ,Floor:%d ,simTime :%lf", &rx, &ry, &rrot, &rfloor, &rtime);
        if (scanned < 4)
            return 1;
    }

    carX = rx;
    carY = ry;
    carRotation = rrot;
    carFloor = rfloor;

    return 1;
}

void random_sim_ordered(int sim, float dt) {
    if (!simRunning || simIndex != sim)
        return;

    open_random_sim_file(sim);
    random_sim_read_and_apply();
    delimitation_of_playground();
}

void update_simulation(float dt) {
    if (!simRunning && simPendingStart) {
        simRunning = true;
        simPendingStart = false;
    }

    switch (simIndex) {
    case 0:
        random_sim_ordered(0, dt);
        break;
    case 1:
        random_sim_ordered(1, dt);
        break;
    case 2:
        random_sim_ordered(2, dt);
        break;
    }
}

void stop_replay_file() {
    if (replay_fp) {
        fclose(replay_fp);
        replay_fp = NULL;
    }
    replay_active = 0;
}
