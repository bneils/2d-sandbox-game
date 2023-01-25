NAME=voxel_sidescroller
CFLAGS=-Wall -Wextra -O2 -lSDL2 -lm
CC=gcc

.PHONY: clean

$(NAME): main.c exit.c render.c world.c
	$(CC) -o $@ $(CFLAGS) $^

clean:
	rm $(NAME) *.o
