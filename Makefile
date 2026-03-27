CC = gcc
CFLAGS_COMMON 	= -std=c2x -D_GNU_SOURCE -I $(INCLUDE_DIR)
CFLAGS_RELEASE 	= -O2 -DNDEBUG -flto -Wall -Wextra
CFLAGS_DEBUG   	= -O0 -ggdb -Wall -Wextra -Werror -fsanitize=address -fno-omit-frame-pointer -DDEBUG
LDFLAGS_RELEASE = -flto
LDFLAGS_DEBUG   = -fsanitize=address

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = .

SOURCES = $(shell find $(SRC_DIR) -type f -name '*.c')
HEADERS = $(shell find $(INCLUDE_DIR) -type f -name '*.h')

# Object files for release and debug (with subdirs)
RELEASE_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/release/%.o,$(SOURCES))
DEBUG_OBJS   = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/debug/%.o,$(SOURCES))

TEMPLATE_SRC = $(wildcacar static/*.html.nex)
TEMPLATE_C 	 =  $(patsubst %.html.nex, build/templates/%.c, $(TEMPLATE_SRC))

EXEC = nexus

build/templates/%.c: static/%.html.nex
	mkdir -p $(dir $@)
		./nexc $< $@

SOURCES += $(TEMPLATE_C)

# Default target: release
all: release

release: $(EXEC)

debug: $(EXEC)_dbg

# Link release
$(EXEC): $(RELEASE_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS_RELEASE)

# Link debug
$(EXEC)_dbg: $(DEBUG_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS_DEBUG)

# Rule to compile release objects
$(BUILD_DIR)/release/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)/release
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_COMMON) $(CFLAGS_RELEASE) -c $< -o $@

# Rule to compile debug objects
$(BUILD_DIR)/debug/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)/debug
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) -c $< -o $@

# Create build directories
$(BUILD_DIR)/release $(BUILD_DIR)/debug:
	mkdir -p $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(EXEC) $(EXEC)_dbg

# Run release
run: $(EXEC)
	./$(EXEC) serve 8080

# Run debug
run_dbg: $(EXEC)_dbg
	./$(EXEC)_dbg serve 8080

compile_commands:
	bear -- make debug

.PHONY: all release debug clean run run_dbg
