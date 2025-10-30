#include "general.h"

void build_path(char *full_path, const char *base_path, const char *filename) {
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "Could not get current working directory\n");
        return;
    }

    if (snprintf(full_path, PATH_MAX, "%s/%s%s", cwd, base_path, filename) >= PATH_MAX) {
        fprintf(stderr, "Path too long!\n");
        full_path[0] = '\0';
    }
}
