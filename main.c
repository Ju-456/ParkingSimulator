#include "general.h"
#include "parking.h"

int main (){

    char full_path_json[PATH_MAX] = {0};
    Parking places[MAX_PLACES];

    build_path(full_path_json, "graph_json/", FILENAME_JSON);
    int num_parking_places = count_number_places(full_path_json);
    printf("num_parking_places = %d\n", num_parking_places);

    if (load_graph_from_json(full_path_json, num_parking_places, places)){
        init_window(full_path_json, num_parking_places, places);
    }
}