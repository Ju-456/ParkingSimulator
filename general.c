#include "general.h"

void build_path(char *full_path, const char *base_path, const char *filename) {
    char cwd[PATH_MAX];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        //printf("Current working directory: %s\n", cwd); 

        char *last_slash = strrchr(cwd, '/');
        if (last_slash != NULL) {
            *last_slash = '\0'; 
        }

        snprintf(full_path, PATH_MAX, "%s/ParkingSimulator/%s%s", cwd, base_path, filename);
        printf("Full path: %s\n", full_path); 
    } else {
        fprintf(stderr, "Could not get current working directory\n");
    }
}

int count_number_places(char full_path_json[]) {
    FILE *file = fopen(full_path_json, "r"); 
    if (file == NULL) {
        fprintf(stderr, "Erreur ouverture fichier JSON\n");
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
            if (fscanf(file, " \"%[^\"]\"", token) == 1) {
                count++;
            }
        }
    }
    fclose(file);
    return count;
}