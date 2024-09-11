CC = gcc
CFLAGS = -std=gnu89 -pedantic -Wall
SOURCES = assembler.c first_pass.c second_pass.c macro_handle.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = assembler

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c assembler.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean