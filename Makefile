NAME=bin/adventure2d
CFLAGS=-Wall -Wextra -O2 -lSDL2 -lm -luuid 
CC=gcc
OBJS=main.o exit.o render.o world.o entity.o hashmap.o

.PHONY: clean

$(NAME): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm $(NAME) *.o
