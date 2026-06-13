TARGET := vip
CC := gcc

SRC_DIR := src
INC_DIR := include

BUILD_DIR := build
BIN_DIR := bin

DEBUG_BUILD_DIR := $(BUILD_DIR)/debug
RELEASE_BUILD_DIR := $(BUILD_DIR)/release

DEBUG_BIN_DIR := $(BIN_DIR)/debug
RELEASE_BIN_DIR := $(BIN_DIR)/release

SRCS := $(wildcard $(SRC_DIR)/*.c)

DEBUG_OBJS := $(SRCS:$(SRC_DIR)/%.c=$(DEBUG_BUILD_DIR)/%.o)
RELEASE_OBJS := $(SRCS:$(SRC_DIR)/%.c=$(RELEASE_BUILD_DIR)/%.o)

WARN_FLAGS := -Wall -Wextra -Wpedantic -Werror
INC_FLAGS := -I$(INC_DIR)
DEP_FLAGS := -MMD -MP

COMMON_FLAGS := $(WARN_FLAGS) $(INC_FLAGS) $(DEP_FLAGS)

CFLAGS_DEBUG := $(COMMON_FLAGS) -g3 -O0 -DDEBUG
CFLAGS_RELEASE := $(COMMON_FLAGS) -O2 -flto -DNDEBUG

LDFLAGS_DEBUG :=
LDFLAGS_RELEASE := -flto

.DEFAULT_GOAL := debug

debug: $(DEBUG_BIN_DIR)/$(TARGET)

run: debug
	@./$(DEBUG_BIN_DIR)/$(TARGET) $(ARGS)

gdb: debug
	@gdb ./$(DEBUG_BIN_DIR)/$(TARGET)

$(DEBUG_BIN_DIR)/$(TARGET): $(DEBUG_OBJS) | $(DEBUG_BIN_DIR)
	$(CC) $(LDFLAGS_DEBUG) $^ -o $@

$(DEBUG_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(DEBUG_BUILD_DIR)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

release: $(RELEASE_BIN_DIR)/$(TARGET)

$(RELEASE_BIN_DIR)/$(TARGET): $(RELEASE_OBJS) | $(RELEASE_BIN_DIR)
	$(CC) $(LDFLAGS_RELEASE) $^ -o $@
	@strip $@

$(RELEASE_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(RELEASE_BUILD_DIR)
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

$(DEBUG_BUILD_DIR) $(RELEASE_BUILD_DIR) $(DEBUG_BIN_DIR) $(RELEASE_BIN_DIR):
	@mkdir -p $@

DEPS := $(wildcard $(DEBUG_BUILD_DIR)/*.d $(RELEASE_BUILD_DIR)/*.d)
-include $(DEPS)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

compile-db:
	@bear -- make -B debug

.PHONY: debug run gdb release clean compile-db
