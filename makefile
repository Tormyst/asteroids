TARGET = asteroids
LIBS = -framework OpenGL -framework GLUT
CC = gcc
CFLAGS = -g -Wall

.PHONY: default run clean

default: run
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -w $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

run: $(TARGET)
	./asteroids

clean:
	-rm -f *.o
	-rm -f $(TARGET)
