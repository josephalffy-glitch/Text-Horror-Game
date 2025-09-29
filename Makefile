# Simple Makefile for C++ game project
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Source files
SOURCES = main.cpp player.cpp ASCII.cpp

# Target executable
TARGET = game.exe

# Default target - compile and create executable
all: $(TARGET)

# Build the game executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Clean up compiled and executable files
clean:
	del /Q *.o $(TARGET) *.exe 2>nul || true

# Phony targets
.PHONY: all run clean