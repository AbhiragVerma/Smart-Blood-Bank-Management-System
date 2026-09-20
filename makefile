CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -Iinclude

TARGET = raktsetu

SRC = src/shell.c \
      src/process.c \
      src/states.c \
      src/scheduler.c \
      src/blood.c \
      src/logger.c

OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)