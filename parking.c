
#define MAX_PLACES 100
#include "parking.h"
#include "general.h"

int count_number_places(char full_path_json[]) {
    FILE *file = fopen(full_path_json, "r"); 
    if (file == NULL) {
        fprintf(stderr, "Error opening JSON file\n");
        return 0;
    }

    int count = 0;
    char c;
    char token[MAX_PLACES];
    int inNodesIds = 0; 

    while ((c = fgetc(file)) != EOF) {
        if (!inNodesIds) {
            if (c == '[') {
                inNodesIds = 1;
            }
            continue;
        }

        if (c == ']') {
            break;
        }

        if (c == '"') {
            ungetc(c, file); 
            if (fscanf(file, " \"%[^\"]\"", token) == 1) { // "%d"
                count++;
            }
        }
    }
    fclose(file);
    return count;
}

void read_id_from_json(const char *full_path_json, Parking places[], int num_parking_places) {
    JSON_Value *root_val = json_parse_file(full_path_json);
    JSON_Object *root_obj = json_value_get_object(root_val);

    if (root_val == NULL) {
        fprintf(stderr, "Error parsing JSON file: %s\n", full_path_json);
        return;
    }

    JSON_Array *ids = json_object_get_array(root_obj, "nodesIds");
    num_parking_places = json_array_get_count(ids);

    for (int i = 0; i < num_parking_places ; i++) {
        const char *id = json_array_get_string(ids, i);
        strcpy(places[i].id, id);
    }

    json_value_free(root_val);
}

int load_graph_from_json(const char *full_path_json, int num_parking_places, Parking places[]) {
    char* strTmp = (char*)malloc(256*sizeof(char));
    strcpy(strTmp,full_path_json);
    //build_path(strTmp, "graph_json/", FILENAME_JSON);

    read_id_from_json(full_path_json, places, num_parking_places);

    JSON_Value *root_val = json_parse_file(full_path_json);
    JSON_Object *root_obj = json_value_get_object(root_val);

    if (root_val == NULL) {
        fprintf(stderr, "Error parsing JSON file: %s\n", full_path_json);
        return 0;
    }    

    JSON_Array *ids = json_object_get_array(root_obj, "nodesIds");
    num_parking_places = json_array_get_count(ids);

    for (int i = 0; i < num_parking_places; i++) {
        const char *id = json_array_get_string(ids, i);
        JSON_Object *node = json_object_get_object(root_obj, id);

        strcpy(places[i].id, id);
        places[i].x = (float)json_object_get_number(node, "x");
        places[i].y = (float)json_object_get_number(node, "y");

        printf("Place %s : (x = %d, y = %d)\n", places[i].id, places[i].x, places[i].y);
    }

    json_value_free(root_val);
    return 1;
}

int init_direction_parking_places(int num_parking_places, Parking places[MAX_PLACES]) {
    int xGroups[MAX_PLACES];  
    int numGroups = 0;

    for (int i = 0; i < num_parking_places; i++) {
        int x = places[i].x;
        int groupIndex = -1;

        for (int k = 0; k < numGroups; k++) {
            if (xGroups[k] == x) {
                groupIndex = k;
                break;
            }
        }

        if (groupIndex == -1) { // if don't find => creation of new group
            xGroups[numGroups] = x;
            groupIndex = numGroups;
            numGroups++;
            printf("Xgroup%d = %d\n", groupIndex + 1, x);
        }

        places[i].direction = groupIndex % 2; //  // even group = 0, odd group = 1
        printf("Place %s → direction = %d\n", places[i].id, places[i].direction);
    }

    printf("\n");
    printf("There are %d different groups (diff columns)\n", numGroups);
    return numGroups;
}

