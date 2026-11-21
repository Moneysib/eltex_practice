CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I.
TARGET = calc
OBJS = main.o calc.o
PLUGINS = plugins/libadd.so \
		plugins/libsubtract.so \
		plugins/libmultiply.so \
		plugins/libdivide.so

all: $(TARGET) $(PLUGINS)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ -ldl

%.o: %.c calc.h operation.h
	$(CC) $(CFLAGS) -c $< -o $@

plugins/lib%.so: plugins/%.c operation.h
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) plugins/*.so

.PHONY: all clean