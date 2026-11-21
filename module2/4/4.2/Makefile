CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = queue
OBJS = main.o queue.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c queue.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean