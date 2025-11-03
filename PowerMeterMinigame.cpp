#include "PowerMeterMinigame.h"
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <chrono> // added for time units

using namespace std;

#ifdef _WIN32
#include <conio.h>
#else
// this is only for mac since i work on mac
#include <unistd.h>
#include <termios.h>
#include <fcntl.h> // added for non-blocking input
#endif

// global variables for mac terminal state, must be used outside functions
#ifndef _WIN32
struct termios old_termios, new_termios;
int old_fcntl;
#endif

// function to check if a key has been pressed
bool PowerMeterMinigame::keyPressed() {
#ifdef _WIN32
    if (_kbhit()) {
        _getch();
        return true;
    }
    return false;
#else
    // mac input logic
    char ch;
    // try to read one byte without blocking
    int bytesRead = read(STDIN_FILENO, &ch, 1);
    
    // if a byte was read, it means a key was pressed
    return bytesRead > 0;
#endif
}

// constructor to set up game variables
PowerMeterMinigame::PowerMeterMinigame() {
    // initialize random seed
    srand(time(NULL));
    meterWidth = 50;
    gameSpeedMs = 60;
    targetZoneWidth = 6;
    // calculate a random starting position for the target zone
    targetZoneStart = 3 + (rand() % (meterWidth - targetZoneWidth - 6));
    targetZoneEnd = targetZoneStart + targetZoneWidth;
    cursorPosition = 0;
    direction = 1; // start moving right
}

// main game loop
bool PowerMeterMinigame::run() {
    // --- MAC/LINUX TERMINAL SETUP START ---
#ifndef _WIN32
    // 1. save old terminal settings
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    
    // 2. set new settings for unbuffered input:
    // disable canonical mode (buffering) and echo
    new_termios.c_lflag &= ~(ICANON | ECHO);
    // set min characters and timeout
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;
    
    // apply new settings
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    
    // 3. set stdin to non-blocking mode
    old_fcntl = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, old_fcntl | O_NONBLOCK);
#endif
    // --- MAC/LINUX TERMINAL SETUP END ---

    // print initial message
    cout << "\n\n  --- power surge detected ---\n";
    cout << "  you run to the fuse box! stabilize the meter!\n";
    this_thread::sleep_for(std::chrono::seconds(1)); 

    while (true) {
        // game ends when any key is pressed
        if (keyPressed()) break; 
        update();
        display();
        // pause for game speed
        this_thread::sleep_for(std::chrono::milliseconds(gameSpeedMs));
    }
    
    // --- MAC/LINUX TERMINAL TEARDOWN START ---
#ifndef _WIN32
    // restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    // restore original fcntl settings
    fcntl(STDIN_FILENO, F_SETFL, old_fcntl);
#endif
    // --- MAC/LINUX TERMINAL TEARDOWN END ---

    bool win = checkWinCondition(cursorPosition);
    cout << "\n\n  >> " << (win ? "nice! power is back online." 
                              : "ah, missed it. a power surge was detected.") << " <<\n";
    // pause before exiting
    this_thread::sleep_for(std::chrono::seconds(2)); 
    return win;
}

// moves the cursor and handles boundary reversal
void PowerMeterMinigame::update() {
    cursorPosition += direction;
    if (cursorPosition <= 0 || cursorPosition >= meterWidth)
        direction *= -1; // reverse direction
}

// draws the game display in the console
void PowerMeterMinigame::display() {
// clear the screen for animation
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    cout << "\n\n";
    cout << "      [=== AUXILIARY POWER CONTROL ===]\n\n";
    cout << "  goal: stop the cursor (^) inside the '=' zone!\n\n";

    cout << "  [";
    // draw the meter bar with '-' for empty and '=' for target zone
    for (int i = 0; i < meterWidth; ++i)
        cout << ((i >= targetZoneStart && i < targetZoneEnd) ? "=" : "-");
    cout << "]\n  ";

    // draw the cursor (^) at its current position
    for (int i = 0; i <= cursorPosition; ++i) cout << " ";
    cout << "^\n\n  [status] press any key to stop the cursor.\n";
}

// checks if the final cursor position is within the target zone
bool PowerMeterMinigame::checkWinCondition(int finalPosition) {
    return finalPosition >= targetZoneStart && finalPosition < targetZoneEnd;
}
