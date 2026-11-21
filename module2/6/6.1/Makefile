CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Ilib
AR = ar
TARGET = phonebook
LIB = lib/libcontacts.a

all: $(TARGET)

$(TARGET): main.o $(LIB)
	$(CC) main.o -o $@ -Llib -lcontacts

$(LIB): lib/contacts.o
	$(AR) rc $@ $^

%.o: %.c lib/contacts.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main.o lib/contacts.o $(LIB) $(TARGET)

.PHONY: all clean