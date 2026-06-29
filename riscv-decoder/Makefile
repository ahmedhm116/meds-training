CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -Iinclude
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

SRC    = $(wildcard $(SRC_DIR)/*.c)
OBJ    = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/riscv-decoder

.PHONY: all clean test debug valgrind

all: dirs $(TARGET)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

debug: CFLAGS += -DDEBUG -O0
debug: all

release: CFLAGS += -O2 -DNDEBUG
release: all

test: all
	./$(TARGET) test/programs/mixed.hex

valgrind: all
	valgrind --leak-check=full ./$(TARGET) test/programs/mixed.hex