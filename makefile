
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = -pthread
TARGET = raktsetu

SRC = src/shell.c \
      src/process.c \
      src/states.c \
      src/scheduler.c \
      src/blood.c \
      src/logger.c \
      src/sync/race_mutex.c \
      src/sync/lock_variable.c \
      src/sync/strict_alternation.c \
      src/sync/peterson.c \
      src/sync/bakery.c \
      src/sync/test_and_set.c \
      src/sync/semaphore.c \
      src/sync/monitor.c \
      src/sync/bounded_buffer.c \
      src/sync/philosophers.c \
      src/sync/reader_writer.c \
      src/sync/sleeping_barber.c \
      src/memory.c \
      src/paging.c \
      src/virtual_memory.c \
      src/bankers.c \
      src/file_allocation.c \
      src/mlq_scheduler.c \
      src/disk_scheduling.c

OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
