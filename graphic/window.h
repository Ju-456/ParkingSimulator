#ifndef WINDOW_MENU_H
#define WINDOW_MENU_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define MAX_FLOOR 2
#define OPEN_DELAY 1.0
#define CLOSE_DELAY 5.0
#define NUM_CARS 6
#define BARRIER_SPEED 180.0f

#include "general/parking_algo.h"
#include <math.h>

typedef struct {
    Texture2D right;
    Texture2D front;
    Texture2D top;
} carOrientation;

typedef enum { CAR_BLACK = 0, CAR_BLUE, CAR_GRAY, CAR_PINK, CAR_RED, CAR_YELLOW } CarColor;

typedef enum { SCREEN_MANUAL_PANEL, SCREEN_RANDOM, SCREEN_MANUAL, SCREEN_DIRECTION, SCREEN_HARD_MANUAL, SCREEN_END } Screen;

// Textures générales
extern Texture2D background;
extern Texture2D free_parking_place;
extern Texture2D busy_parking_place;
extern Texture2D panel_menu;

extern Texture2D entrance_barrier;
extern Texture2D entrance_ticket_dispenser;
extern Texture2D exit_pay_station;
extern Texture2D exit_barrier;
extern Texture2D barrier_wall;

extern Texture2D floor_exit;
extern Texture2D floor_indicator[3];

extern Texture2D PC;
extern Texture2D car_preview;

// Voitures
extern carOrientation carOrient[NUM_CARS];

// États du jeu
extern bool gameFinished;
extern bool insideParking;
extern bool controlsUnlocked;

// Parking
extern int currentFloor;
extern int carFloor;
extern Rectangle btnRandom;
extern Rectangle btnManual;
extern Rectangle btnHardManual;
extern Rectangle btnReturn;

extern Rectangle destPreviewLevel;
extern Rectangle destNextLevel;

extern int chosenCar;

extern Rectangle srcMode;
extern Rectangle srcPreview;
extern Rectangle srcArrow;
extern Rectangle srcReturn;
extern Rectangle destReturn;
extern Vector2 origin;
extern Vector2 origin1;

// Déplacements voiture
extern float carX;
extern float carY;
extern float carStep;
extern float carRotation;

// Couleurs
extern Color parkingBlue;
extern Color parkingRed;
extern Color parkingGreen;
extern Color brightGreen;
extern Color brightBlue;

// Tickets / paiement / barrières
extern Rectangle btnTicket;
extern Rectangle btnPay;

extern float entranceAngle;
extern float entranceTargetAngle;
extern float exitAngle;
extern float exitTargetAngle;

extern int entranceState;
extern int exitState;

extern double entranceTriggerTime;
extern double exitTriggerTime;
extern double entranceOpenTime;
extern double exitOpenTime;

extern int ticket;
extern int payment;

// Floor change requests
extern bool floorChangeRequestedUp;
extern bool floorChangeRequestedDown;

// Simulation
extern bool randomSimulationStarted;
extern bool simPendingStart;
extern bool simRunning;

// menu part
void panel();
void init_manual_panel_menu();
void manual_panel_menu(Font font);

// barrier part
void update_barrier_angles();
void barrier_management(int barrierType, int barrier_state);
void handle_automatic_opening();
void draw_entrance_barrier();
void draw_exit_barrier();
bool exit_is_passable();

// parking part
void draw_parking_places(int n, Parking places[]);
void draw_parked_cars(int n, Parking places[]);
void init_parking_state(int n, Parking places[]);
void reset_parking_state(Parking places[], int *num_parking_places);
bool check_collision_with_parking(float carX, float carY, Parking places[], int n);

// floor part
void request_floor_change(int direction, Parking places[], int *num_parking_places);
void reload_floor(int floor, Parking places[], int *num_parking_places);
void draw_floor();
void delimitation_of_playground();

// texture gestion
void load_textures();
void unload_textures();

// RANDOM MODE PART :
// to create a scenario for a random simu
void open_new_sim_file_session();
void close_sim_file_session();
void collect_and_save_simulation_data_auto(float dt);
void request_simulation_start(int sim);
void select_random_car();

// to apply created  scenarios for a random simu
void random_sim_ordered(int sim, float dt);
void update_simulation(float dt);
void stop_replay_file();
/*---------------------------------------------*/

// hard and manual mode part
void choose_your_car(Font font);
void choose_your_car_condition();
void update_car_position(float dt, Parking places[], int n);

// buttons
void draw_buttons_direction(Texture2D PC);
void draw_floor_arrows(Texture2D PC, Rectangle srcArrow, Rectangle prev, Rectangle next, int floor, bool enabled);
void draw_return_arrow(Texture2D PC, Rectangle srcReturn, Rectangle destReturn, bool enabled);
void draw_ticket_pay_buttons(Font font, bool enabled);
void handle_station_buttons_click(Vector2 mouse, bool enabled);

// general functions : in window.c
bool game_mode_selected(Screen s);
Color disabled_tint(Color base, bool enabled);
void place_car_at_start_pos();

// main window
void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[]);

#endif