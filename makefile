CC = gcc

CFLAGS = -Wall -I./ -I/home/ju/raylib/src
LDFLAGS = -L/home/ju/raylib/src -lraylib -lm -lpthread -ldl -lX11

# CFLAGS = -Wall -I./ -I/opt/homebrew/opt/raylib/include
# LDFLAGS = -L/opt/homebrew/opt/raylib/lib -lraylib \
#           -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Files
SRCS = main.c general/utils.c general/parking_algo.c general/parson.c graphic/window.c graphic/barriers.c graphic/buttons.c graphic/floor.c graphic/hard_and_manual_mode.c graphic/menu.c graphic/parking_graphic.c graphic/random_mode.c graphic/texture_gestion.c
OBJS = $(SRCS:.c=.o)

HEADERS = general/utils.h general/parking_algo.h general/parson.h graphic/window.h
EXEC = parkingSimulator

# Build executable
$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile .c to .o
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(EXEC)

# Rebuild from scratch
rebuild: clean $(EXEC)
