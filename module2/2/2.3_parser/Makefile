CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = calc
OBJS = main.o calc.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c calc.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean