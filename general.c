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
