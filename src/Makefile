PROJECT_NAME=tbd

CURR_DIR=$(shell pwd)
LIB_DIR=$(CURR_DIR)/Libraries


glad:
	gcc -c 'Libraries/glad/glad.c' -o 'Libraries/glad/glad.o'
	chmod u+x $(LIB_DIR)/glad/glad.o

clean:
	rm 'Libraries/glad/glad.o'
	rm .a.out
	rm main.o

main_sdl: glad
	gcc -c main_sdl.c -o main.o

all: main_sdl
	gcc main.o $(LIB_DIR)/glad/glad.o -o a.out -lSDL2 -ldl -lm -lGl
