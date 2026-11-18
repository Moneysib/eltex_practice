CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_XOPEN_SOURCE=700
TARGET = pubsub
OBJS   = main.o broker.o client.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c pubsub.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean