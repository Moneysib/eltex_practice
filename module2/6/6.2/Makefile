CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Ilib
TARGET = phonebook
LIB = lib/libcontacts.so

all: $(TARGET)

$(TARGET): main.o $(LIB)
	$(CC) main.o -o $@ -Llib -lcontacts -Wl,-rpath,'$$ORIGIN/lib'

$(LIB): lib/contacts.o
	$(CC) -shared $^ -o $@

lib/contacts.o: lib/contacts.c lib/contacts.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

main.o: main.c lib/contacts.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main.o lib/contacts.o $(LIB) $(TARGET)

.PHONY: all clean