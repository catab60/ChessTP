# List all your source files here
SRC = main.c chess.c
# The name of your final program
OBJ = chess

CC = gcc
CFLAGS = -Wall -std=c99
# -I. tells the compiler to look in the current directory for header files (.h)
INCLUDES = -I.
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all:
	$(CC) $(SRC) -o $(OBJ) $(CFLAGS) $(INCLUDES) $(LIBS)

clean:
	rm -f $(OBJ)