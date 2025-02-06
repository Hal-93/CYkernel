CC      := gcc
CFLAGS  := -g -Wall -Wextra -fdiagnostics-color=always

SRCS    := $(wildcard *.c)
OBJS    := $(SRCS:.c=.o)

TARGET  := app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean