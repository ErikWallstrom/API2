CC = gcc
WARNINGS = -Wall -Wextra -Wshadow -Wstrict-prototypes -Wdouble-promotion -Wjump-misses-init -Wnull-dereference -Wrestrict -Wlogical-op -Wduplicated-branches -Wduplicated-cond
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lm

debug:
	$(CC) *.c $(WARNINGS) -fplan9-extensions -Og -g3 $(LIBS)
release:
	$(CC) *.c $(WARNINGS) -fplan9-extensions -march=native -O2 -DNDEBUG $(LIBS)
