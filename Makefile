NAME=bin/sandbox_game
CFLAGS=-Wall -Wextra -O0 -lSDL2 -lm -luuid -g
CC=gcc
OBJS=main.o exit.o render.o world.o entity.o hashmap.o event.o \
     SuperFastHash.o physics.o globals.o
TESTS=silent_chunk_creation fill hashmap_put hashmap_iterate hashmap_remove

.PHONY: clean run fresh test

$(NAME): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^

%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $<

fresh: clean $(NAME)

run: $(NAME)
	$(NAME)

test: $(patsubst %,test_%,$(TESTS))
	@for test in $^ ; do \
		printf "%-40s: " "$$test" ; \
		./$$test ; \
	done ; \
	exit 0

test_%: tests/%.o $(patsubst main.o,,$(OBJS))
	$(CC) -o $@ $(CFLAGS) $^

clean:
	rm $(NAME) *.o test_* | exit 0

