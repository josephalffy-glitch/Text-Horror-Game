# Compiler and flags
CXX = g++
CXXFLAGS = -Iextern/imgui -Iglfw/include -O2 -std=c++17 -Wall

# Platform-specific libraries
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    # macOS
    LIBS = -lglfw \
           -framework OpenGL \
           -framework Cocoa \
           -framework IOKit \
           -framework CoreFoundation
else
    # Windows (MinGW)
    LIBS = -Lglfw/include/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32
endif

# Source files
SRCS = main.cpp \
       player.cpp \
       GameEngine.cpp \
       Room.cpp \
       PowerMeterMinigame.cpp

IMGUI_SRCS = extern/imgui/imgui.cpp \
             extern/imgui/imgui_draw.cpp \
             extern/imgui/imgui_tables.cpp \
             extern/imgui/imgui_widgets.cpp \
             extern/imgui/backends/imgui_impl_glfw.cpp \
             extern/imgui/backends/imgui_impl_opengl3.cpp

# Object files and target
OBJS = $(SRCS:.cpp=.o)
IMGUI_OBJS = $(IMGUI_SRCS:.cpp=.o)
TARGET = Game.exe


# Default build
all: $(TARGET)

# Compile all .cpp files to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link all object files into the executable
$(TARGET): $(OBJS) $(IMGUI_OBJS)
	$(CXX) $(OBJS) $(IMGUI_OBJS) $(LIBS) -o $(TARGET)

# Run the game
run: $(TARGET)
	$(TARGET)

# Clean build files
clean:
ifeq ($(OS),Windows_NT)
	del /Q *.o *.exe *.gch 2>nul
	for /R extern\imgui %%f in (*.o) do del "%%f" 2>nul
else
	rm -f *.o *.gch $(TARGET)
	find extern/imgui -name "*.o" -delete
endif

.PHONY: all run clean
