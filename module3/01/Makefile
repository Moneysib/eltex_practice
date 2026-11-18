CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
TARGET = filecopy
OBJS   = main.o transfer.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c transfer.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
