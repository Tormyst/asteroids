all: run

build:
	gcc asteroids.c glutTime.c displayFont.c -framework OpenGL -framework GLUT -w -o asteroids
run: build
	./asteroids