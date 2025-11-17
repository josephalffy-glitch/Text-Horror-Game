# Compiler and flags
CXX = g++
CXXFLAGS = -Iextern/imgui -Iglfw/include -O2 -std=c++17 -Wall

# Platform-specific libraries
# Use OS variable for Windows detection, then check for Darwin if available
ifeq ($(OS), Windows_NT)
	# Windows (MinGW)
	LIBS = -Lglfw/include/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32
	TARGET = Game.exe
	PLATFORM = Windows
else
	# Try to detect Unix-like systems
	UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
	ifeq ($(UNAME_S), Darwin)
		# macOS
		LIBS = -lglfw \
			-framework OpenGL \
			-framework Cocoa \
			-framework IOKit \
			-framework CoreFoundation
		TARGET = Game
		PLATFORM = macOS
		ifeq ($(shell which clang++ 2>/dev/null),)
			CXX = g++
		else
			CXX = clang++
		endif
	else
		# Linux or other Unix
		CXXFLAGS += -Iopengl/include
		LIBS = -lglfw -lGL -lm -ldl -lpthread
		TARGET = Game
		PLATFORM = Linux/Unix
	endif
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

# Default build
all: $(TARGET)

# Compile all .cpp files to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link all object files into the executable
$(TARGET): $(OBJS) $(IMGUI_OBJS)
	$(CXX) $(OBJS) $(IMGUI_OBJS) $(LIBS) -o $(TARGET)

# Run the game (platform-specific)
run: $(TARGET)
ifeq ($(OS), Windows_NT)
	$(TARGET)
else
	./$(TARGET)
endif

# Clean build files (cross-platform)
clean:
ifeq ($(OS), Windows_NT)
	@echo Cleaning Windows build files...
	-del /Q *.o *.exe *.gch 2>nul || echo Done
	-for /R extern\imgui %%f in (*.o) do del "%%f" 2>nul || echo Done
else
	@echo Cleaning Unix-like build files...
	rm -f *.o *.gch $(TARGET)
	find extern/imgui -name "*.o" -delete 2>/dev/null || true
endif

# Help target show platform info
info:
	@echo "Platform: $(PLATFORM)"
	@echo "OS: $(OS)"
	@echo "Compiler: $(CXX)"
	@echo "Target: $(TARGET)"
	@echo "Libraries: $(LIBS)"

.PHONY: all run clean info
