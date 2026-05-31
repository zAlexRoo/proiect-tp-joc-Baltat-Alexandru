# Makefile - Tic-Tac-Toe cu Raylib

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
LIBS    = -lraylib -lm -lX11 -lXrandr -lXinerama -lXi -lXcursor -lpthread -ldl
TARGET  = tictactoe
SRCS    = main.c board.c ai.c
OBJS    = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean run rebuild