#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>     
#include <string.h>  
#include <stdlib.h>    
#include <limits.h>    
#include <unistd.h>     
#include "raylib.h"

#define FILENAME_JSON "graph1.json"
#define MAX_PLACES 100

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

void build_path(char *full_path, const char *base_path, const char *filename);

#endif
