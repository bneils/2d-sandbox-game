NAME=bin/sandbox_game
CFLAGS=-Wall -Wextra -O0 -lSDL2 -lm -luuid -g
CC=gcc
OBJS=main.o exit.o render.o world.o entity.o hashmap.o

.PHONY: clean

$(NAME): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm $(NAME) *.o
