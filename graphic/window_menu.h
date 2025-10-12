#ifndef WINDOW_MENU_H
#define WINDOW_MENU_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define MAX_FLOOR 2
#define OPEN_DELAY 1.0
#define CLOSE_DELAY 5.0
#define NUM_CARS 6

#include "parking.h"
#include <math.h>

typedef struct {
    Texture2D right;
    Texture2D front;
    Texture2D top;
} carOrientation;

typedef enum { CAR_BLACK = 0, CAR_BLUE, CAR_GRAY, CAR_PINK, CAR_RED, CAR_YELLOW } CarColor;

typedef enum {
    SCREEN_MANUAL_PANEL,
    SCREEN_RANDOM,
    SCREEN_MANUAL,
    SCREEN_DIRECTION,
    SCREEN_HARD_MANUAL
} Screen;

static inline bool game_mode_selected(Screen s);
// static inline bool entrance_is_passable();
// static inline bool exit_is_passable();
void draw_parking_places(int n, Parking places[]);
void panel();
void update_barrier_angles();
void barrier_management(int barrier_type, int barrier_state);

void handle_automatic_opening();

void draw_entrance_barrier();
void draw_exit_barrier();

void reload_floor(int floor, Parking places[], int *num_parking_places);
void draw_floor();

void init_manual_panel_menu();
void manual_panel_menu(Font font);

void load_textures();
void choose_your_car(Font font);
void choose_your_car_condition();
void update_car_position(float dt);
void delimitation_of_playground();
void place_car_at_start_pos();
void draw_buttons_direction(Texture2D PC);

void draw_floor_arrows(Texture2D PC, Rectangle srcArrow, Rectangle prev, Rectangle next, int floor,
                       bool enabled);
void draw_return_arrow(Texture2D PC, Rectangle srcReturn, Rectangle destReturn, bool enabled);
static inline Color disabled_tint(Color base, bool enabled);
void draw_ticket_pay_buttons(Font font, bool enabled);
void handle_station_buttons_click(Vector2 mouse, bool enabled);

void unload_textures();

void init_window_parking(const char *full_path_json, int num_parking_places, Parking places[]);

#endif