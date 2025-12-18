# Makefile for Adventure Quest Engine
# C++ Adventure Quest Engine Build Configuration

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS = 

# Debug and Release flags
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(SRC_DIR)/Combat.cpp \
          $(SRC_DIR)/Events.cpp \
          $(SRC_DIR)/FileIO.cpp \
          $(SRC_DIR)/Inventory.cpp \
          $(SRC_DIR)/MapGenerator.cpp \
          $(SRC_DIR)/Utils.cpp \
          $(SRC_DIR)/GameEngine.cpp \
          $(SRC_DIR)/SkillTree.cpp \
          $(SRC_DIR)/Crafting.cpp \
          $(SRC_DIR)/Companion.cpp \
          $(SRC_DIR)/Achievement.cpp \
          $(SRC_DIR)/UIEnhanced.cpp \
          main.cpp

# Object files
OBJECTS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))

# Executable name
TARGET = $(BIN_DIR)/AdventureQuestEngine

# Default target
all: prebuild directories $(TARGET)

# Prebuild step to fix potential clock skew by updating file timestamps
.PHONY: prebuild fix-timestamps
prebuild: fix-timestamps

fix-timestamps:
	@echo "Synchronizing file timestamps to avoid clock skew warnings..."
	@find $(SRC_DIR) $(INCLUDE_DIR) -type f -exec touch {} + || true


# Create directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

# Build executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean all

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: clean all

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o
	rm -f $(TARGET)
	@echo "Clean complete"

# Clean everything including directories
distclean: clean
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Install (optional)
install: $(TARGET)
	@echo "Installing to /usr/local/bin (requires sudo)"
	sudo cp $(TARGET) /usr/local/bin/

# Help
help:
	@echo "Adventure Quest Engine - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build the project (default)"
	@echo "  debug      - Build with debug symbols"
	@echo "  release    - Build optimized release version"
	@echo "  clean      - Remove build artifacts"
	@echo "  distclean  - Remove all generated files and directories"
	@echo "  run        - Build and run the game"
	@echo "  install    - Install to system (requires sudo)"
	@echo "  help       - Show this help message"

.PHONY: all directories debug release clean distclean run install help
