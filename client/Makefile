# Compiler and flags
CXX = c++
CXXFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -Werror -pthread -Iinclude/
LDFLAGS = -L/usr/lib -lstdc++ -lm -lncursesw -lSDL2

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/objects
APP_DIR = $(BUILD_DIR)/apps

# Source files and object files
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/sdltry.cpp
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Target executable
TARGET = $(APP_DIR)/tetris_client

# Build rules
all: $(TARGET)

# Linking step
$(TARGET): $(OBJS)
	mkdir -p $(APP_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compilation step
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)/$(dir $*)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

# PHONY targets
.PHONY: all clean
