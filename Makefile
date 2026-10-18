# makefile genuinely frying my brain what am i doing anymore

CC := gcc

CFLAGS := -std=c99 -Wall
LDLIBS :=

ifeq ($(OS),Windows_NT)
	CFLAGS := -I$(SDL2_PATH)/include/SDL2
	LDLIBS += -L$(SDL2_PATH)/lib -lSDL2
else
	CFLAGS += $(shell sdl2-config --cflags)
	LDLIBS += $(shell sdl2-config --libs)
endif

SRC_DIR := src
BUILD_DIR := build

HDRS := $(wildcard $(SRC_DIR)/*.h)
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

EXEC := $(BUILD_DIR)/chip8

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

all: $(EXEC)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean