CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = phonebook
OBJS = main.o contacts.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c contacts.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean