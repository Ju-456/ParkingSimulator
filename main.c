#include "general.h"

int main (){

    char full_path_json[PATH_MAX] = {0};

    build_path(full_path_json, "graph_json/", FILENAME_JSON);
    int num_parking_places = count_number_places(full_path_json);
    printf("num_parking_places = %d\n", num_parking_places);
    
}