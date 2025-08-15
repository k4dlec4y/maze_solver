CC = gcc
CFLAGS = -std=c99 -c -Wall -Wextra -Iinclude

SRC_DIR=src
BUILD_DIR=build
TARGET=$(BUILD_DIR)/maze

SRCS = main.c maze.c solve.c check.c queue.c
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -std=c99 $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)/
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/:
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)/

.PHONY = all clean
