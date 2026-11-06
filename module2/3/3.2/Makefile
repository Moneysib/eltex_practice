CC     = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = netsim
OBJS   = main.o ipnet.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c ipnet.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean