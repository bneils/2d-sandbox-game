NAME=untitled
CFLAGS=-Wall -Wextra -O2 -lSDL2
CC=gcc

$(NAME): main.c
	$(CC) -o $@ $(CFLAGS) $^
