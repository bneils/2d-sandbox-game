NAME=bin/sandbox_game
CFLAGS=-Wall -Wextra -O2 -lSDL2 -lm -luuid -g
CC=gcc
OBJS=main.o exit.o render.o world.o entity.o hashmap.o event.o \
     SuperFastHash.o

.PHONY: clean run fresh

$(NAME): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

fresh: clean $(NAME)

run: $(NAME)
	$(NAME)

clean:
	rm $(NAME) *.o
