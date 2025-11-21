# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -Iextern/imgui -Iextern/opengl/include -O2 -std=c++17 -Wall

# Platform-specific settings
ifeq ($(OS), Windows_NT)
    # Windows
    PLATFORM = Windows
    TARGET = Game.exe
    CXXFLAGS += -Iglfw/include
    LIBS = -Lglfw/include/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32
else
    # Try to detect Unix-like systems
    UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
    ifeq ($(UNAME_S), Darwin)
        # macOS
        PLATFORM = macOS
        TARGET = Game
        LIBS = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreFoundation
        ifeq ($(shell which clang++ 2>/dev/null),)
            CXX = g++
        else
            CXX = clang++
        endif
    else
        # Linux - Try to use any available GLFW
        PLATFORM = Linux/Unix
        TARGET = Game
        # Try different GLFW library names
        GLFW_LIB := $(shell pkg-config --libs glfw3 2>/dev/null || echo "-lglfw")
        GL_LIBS := $(shell pkg-config --libs gl 2>/dev/null || echo "-lGL")
        LIBS = $(GLFW_LIB) $(GL_LIBS) -lm -ldl -lpthread
    endif
endif

# Your game source files only - no GLFW compilation
GAME_SRCS = main.cpp \
            player.cpp \
            GameEngine.cpp \
            Room.cpp \
            PowerMeterMinigame.cpp \
            LoadGame.cpp

# ImGui source files
IMGUI_SRCS = extern/imgui/imgui.cpp \
            extern/imgui/imgui_draw.cpp \
            extern/imgui/imgui_tables.cpp \
            extern/imgui/imgui_widgets.cpp \
            extern/imgui/backends/imgui_impl_glfw.cpp \
            extern/imgui/backends/imgui_impl_opengl3.cpp

# Object files
GAME_OBJS = $(GAME_SRCS:.cpp=.o)
IMGUI_OBJS = $(IMGUI_SRCS:.cpp=.o)

# Default build
all: $(TARGET)

# Compile C++ files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link all object files
$(TARGET): $(GAME_OBJS) $(IMGUI_OBJS)
	$(CXX) $(GAME_OBJS) $(IMGUI_OBJS) $(LIBS) -o $(TARGET)

# Run the game
run: $(TARGET)
ifeq ($(OS), Windows_NT)
	$(TARGET)
else
	./$(TARGET)
endif

# Clean build files
clean:
ifeq ($(OS), Windows_NT)
	@echo Cleaning Windows build files...
	-del /Q *.o *.exe *.gch 2>nul || echo Done
	-for /R extern %%f in (*.o) do del "%%f" 2>nul || echo Done
else
	@echo Cleaning Unix-like build files...
	rm -f *.o *.gch $(TARGET)
	find extern -name "*.o" -delete 2>/dev/null || true
endif

# Show platform info
info:
	@echo "Platform: $(PLATFORM)"
	@echo "OS: $(OS)"
	@echo "Compiler: $(CXX)"
	@echo "Target: $(TARGET)"
	@echo "Libraries: $(LIBS)"

# Install dependencies help
deps:
	@echo "To install dependencies:"
	@echo "Ubuntu/Debian: apt install libglfw3-dev libgl1-mesa-dev"
	@echo "Fedora: dnf install glfw-devel mesa-libGL-devel"
	@echo "Arch: pacman -S glfw-wayland mesa"
	@echo "macOS: brew install glfw"

.PHONY: all run clean info deps
