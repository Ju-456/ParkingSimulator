#include "general/parking_algo.h"
#include "general/utils.h"
#include "graphic/window.h"
#include <time.h>
int main() {

    char full_path_json[PATH_MAX] = {0};
    Parking places[MAX_PLACES];

    build_path(full_path_json, "graph_json/", FILENAME_JSON);
    int num_parking_places = count_number_places(full_path_json);

    srand(time(NULL));
    if (load_graph_from_json(full_path_json, num_parking_places, places)) {
        init_direction_parking_places(num_parking_places, places);
        init_window_parking(full_path_json, num_parking_places, places);
    }
}