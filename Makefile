NAME=voxel_sidescroller
CFLAGS=-Wall -Wextra -O2 -lSDL2 -lm
CC=gcc
OBJ=main.o exit.o render.o world.o

.PHONY: clean

$(NAME): $(OBJ)
	$(CC) -o $@ $(CFLAGS) $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm $(NAME) *.o
