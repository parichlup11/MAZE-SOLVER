CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lm

TARGET = maze

SOURCES = main.c maze.c queue.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean