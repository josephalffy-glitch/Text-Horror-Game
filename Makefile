# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -Iextern/imgui -Iextern/glfw/include -Iopengl/include -O2 -std=c++17 -Wall
CFLAGS = -Iextern/glfw/include -Iopengl/include -O2 -Wall

# Platform-specific settings
ifeq ($(OS), Windows_NT)
    # Windows
    PLATFORM = Windows
    TARGET = Game.exe
    GLFW_PLATFORM_SRCS = extern/glfw/src/win32_init.c \
                        extern/glfw/src/win32_joystick.c \
                        extern/glfw/src/win32_monitor.c \
                        extern/glfw/src/win32_time.c \
                        extern/glfw/src/win32_thread.c \
                        extern/glfw/src/win32_window.c \
                        extern/glfw/src/wgl_context.c \
                        extern/glfw/src/egl_context.c \
                        extern/glfw/src/osmesa_context.c
    GLFW_CFLAGS = -D_GLFW_WIN32
    LIBS = -lopengl32 -lgdi32 -luser32 -lkernel32
else
    # Try to detect Unix-like systems
    UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
    ifeq ($(UNAME_S), Darwin)
        # macOS
        PLATFORM = macOS
        TARGET = Game
        GLFW_PLATFORM_SRCS = extern/glfw/src/cocoa_init.m \
                            extern/glfw/src/cocoa_joystick.m \
                            extern/glfw/src/cocoa_monitor.m \
                            extern/glfw/src/cocoa_time.c \
                            extern/glfw/src/posix_thread.c \
                            extern/glfw/src/cocoa_window.m \
                            extern/glfw/src/nsgl_context.m \
                            extern/glfw/src/egl_context.c \
                            extern/glfw/src/osmesa_context.c
        GLFW_CFLAGS = -D_GLFW_COCOA
        LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreFoundation
        ifeq ($(shell which clang++ 2>/dev/null),)
            CXX = g++
            CC = gcc
        else
            CXX = clang++
            CC = clang
        endif
    else
        # Linux
        PLATFORM = Linux/Unix
        TARGET = Game
        GLFW_PLATFORM_SRCS = extern/glfw/src/x11_init.c \
                            extern/glfw/src/x11_monitor.c \
                            extern/glfw/src/x11_window.c \
                            extern/glfw/src/xkb_unicode.c \
                            extern/glfw/src/posix_time.c \
                            extern/glfw/src/posix_thread.c \
                            extern/glfw/src/glx_context.c \
                            extern/glfw/src/egl_context.c \
                            extern/glfw/src/osmesa_context.c \
                            extern/glfw/src/linux_joystick.c
        GLFW_CFLAGS = -D_GLFW_X11
        LIBS = -lGL -lm -ldl -lpthread -lX11 -lXrandr -lXinerama -lXcursor -lXi
    endif
endif

# Common GLFW source files (platform-independent)
GLFW_COMMON_SRCS = extern/glfw/src/context.c \
                   extern/glfw/src/init.c \
                   extern/glfw/src/input.c \
                   extern/glfw/src/monitor.c \
                   extern/glfw/src/vulkan.c \
                   extern/glfw/src/window.c

# All GLFW sources
GLFW_SRCS = $(GLFW_COMMON_SRCS) $(GLFW_PLATFORM_SRCS)

# Your game source files
GAME_SRCS = main.cpp \
            player.cpp \
            GameEngine.cpp \
            Room.cpp \
            PowerMeterMinigame.cpp

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
GLFW_OBJS = $(GLFW_SRCS:.c=.o)

# Handle .m files for macOS
ifeq ($(UNAME_S), Darwin)
    GLFW_OBJS := $(GLFW_OBJS:.m=.o)
endif

# Default build
all: $(TARGET)

# Compile C++ files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C files (GLFW)
%.o: %.c
	$(CC) $(CFLAGS) $(GLFW_CFLAGS) -c $< -o $@

# Compile Objective-C files (macOS only)
%.o: %.m
	$(CC) $(CFLAGS) $(GLFW_CFLAGS) -c $< -o $@

# Link all object files
$(TARGET): $(GAME_OBJS) $(IMGUI_OBJS) $(GLFW_OBJS)
	$(CXX) $(GAME_OBJS) $(IMGUI_OBJS) $(GLFW_OBJS) $(LIBS) -o $(TARGET)

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
	@echo "Compiler: $(CXX) / $(CC)"
	@echo "Target: $(TARGET)"
	@echo "Libraries: $(LIBS)"
	@echo "GLFW Platform Sources: $(GLFW_PLATFORM_SRCS)"

.PHONY: all run clean info
