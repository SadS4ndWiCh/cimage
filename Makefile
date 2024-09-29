CC = gcc
CINCLUDES = -I./includes
CFLAGS = -Wall -Wextra -pedantic

SRCS = $(wildcard ./src/*.c)

all:
	@mkdir -p bin
	@$(CC) $(SRCS) $(CINCLUDES) $(CFLAGS) -o ./bin/cimage