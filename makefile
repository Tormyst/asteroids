TARGET = asteroids
LINUX_LIBS = -lGL -lGLU -lglut -lm
APPLE_LIBS = -framework OpenGL -framework GLUT
CC = gcc
CFLAGS = -g -Wall

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	LIBS += $(LINUX_LIBS)
endif
ifeq ($(UNAME), Darwin)
	LIBS += $(APPLE_LIBS)
endif


.PHONY: default run clean

default: run
all: default

OBJECTS = $(patsubst %.c, .objectFiles/%.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

.objectFiles/%.o: %.c $(HEADERS)
	mkdir -p .objectFiles
	$(CC) $(CFLAGS) -c -w $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

run: $(TARGET)
	./asteroids

clean:
	-rm -f *.o
	-rm -f .objectFiles
	-rm -f $(TARGET)
