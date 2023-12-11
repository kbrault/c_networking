# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Source directory
SRC_DIR = src

# Target directory
TARGET_DIR = target

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(TARGET_DIR)/%.o,$(SRC_FILES))

# Executables
EXECUTABLES = basic_http_server multithread_http_server basic_ftp_server

# Default rule: build all executables
all: $(EXECUTABLES)

# Rule to build each executable
basic_http_server: $(TARGET_DIR)/basic_http_server
multithread_http_server: $(TARGET_DIR)/multithread_http_server
basic_ftp_server: $(TARGET_DIR)/basic_ftp_server

# Rule to build each executable from its object file
$(TARGET_DIR)/%: $(OBJ_FILES)
	$(CC) $(CFLAGS) $@.o -o $@

# Rule to compile source files into object files
$(TARGET_DIR)/%.o: $(SRC_DIR)/%.c | $(TARGET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create the target directory if it doesn't exist
$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Clean rule: remove generated files
clean:
	rm -rf $(TARGET_DIR)
	rm -f $(EXECUTABLES)

.PHONY: all clean