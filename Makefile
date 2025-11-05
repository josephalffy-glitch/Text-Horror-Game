# =========================
# Project Makefile for Windows + MinGW
# =========================

# Source files
SRCS = main.cpp player.cpp GameEngine.cpp \
       extern/imgui/imgui.cpp \
       extern/imgui/imgui_draw.cpp \
       extern/imgui/imgui_tables.cpp \
       extern/imgui/imgui_widgets.cpp \
       extern/imgui/backends/imgui_impl_glfw.cpp \
       extern/imgui/backends/imgui_impl_opengl3.cpp

# Include paths
CXXFLAGS = -Iextern/imgui -Iextern/imgui/backends -IC:/glfw/glfw-3.4.bin.WIN64/include

# Library paths and libraries
LIBS = -LC:/glfw/glfw-3.4.bin.WIN64/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32

# Output executable
TARGET = Game.exe

# =========================
# Targets
# =========================

# Build the project
final:
	g++ $(SRCS) $(CXXFLAGS) $(LIBS) -o $(TARGET)

# Run the game
run: final
	$(TARGET)

# Clean object files and executable
clean:
	del /Q *.o *.exe *.gch 2>nul

.PHONY: final run clean
