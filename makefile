CC = gcc

CFLAGS = -Wall -I./ -I/home/ju456/raylib/src
LDFLAGS = -L/home/ju456/raylib/src -lraylib -lm -lpthread -ldl -lX11

# Files
SRCS = main.c general.c graphic/window_menu.c parking.c parson.c
OBJS = $(SRCS:.c=.o)

HEADERS = general.h graphic/window_menu.h parking.h parson.h
EXEC = car

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
