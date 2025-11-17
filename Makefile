# Compiler and flags
CXX = g++
CXXFLAGS = -Iextern/imgui -Iglfw/include -O2 -std=c++17 -Wall

# Library paths and linked libraries (MAC/LINUX COMPATIBLE)
LIBS = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreFoundation

# ------------------------------------------
# Source files
# ------------------------------------------
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

# Build rules
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
	del /Q *.o *.exe *.gch 2>nul
	for /R extern\imgui %%f in (*.o) do del "%%f" 2>nul

.PHONY: all run clean
