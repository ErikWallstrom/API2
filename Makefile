debug:
	gcc *.c -Wall -Wextra -Wshadow -Wno-format-nonliteral -fplan9-extensions -Og -g3 -lSDL2 -lSDL2_image -lSDL2_ttf -lm

release:
	gcc *.c -Wall -Wextra -Wshadow -Wno-format-nonliteral -fplan9-extensions -march=native -O2 -DNDEBUG -lSDL2 -lSDL2_image -lSDL2_ttf -lm
