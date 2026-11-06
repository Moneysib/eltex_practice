CC     = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = perms
OBJS   = main.o perms.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c perms.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean