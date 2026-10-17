# makefile genuinely frying my brain what am i doing anymore

CC := gcc

CFLAGS := $(shell sdl2-config --cflags) -ggdb3 -O0 --std=c99 -Wall
LDLIBS := $(shell sdl2-config --libs) -lSDL2_image -lm

SRC_DIR := src
BUILD_DIR := build

HDRS := $(wildcard $(SRC_DIR)/*.h)
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

EXEC := $(BUILD_DIR)/chip8
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

clean:
	rm -rf $(EXEC) $(OBJS)

.PHONY: all clean