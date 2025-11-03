#include "PowerMeterMinigame.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono> // for time like milliseconds
#include <cstdlib>
#include <ctime>

using namespace std;

#ifdef _WIN32
#include <conio.h>
#else
// headers for keyboard input on mac/linux
#include <unistd.h>
#include <termios.h>
#endif
// check if a key is pressed on mac/linux
#ifndef _WIN32
bool PowerMeterMinigame::keyPressed() {
    struct termios oldt, newt;
    int bytesRead;
    char ch;
    // save terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    // change terminal settings so we can read one key press
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    // see if a key was actually pressed
    bytesRead = read(STDIN_FILENO, &ch, 1);

    //put terminal settings back to normal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return bytesRead > 0; 
}
#endif
// check if a key is pressed on windows
#ifdef _WIN32
bool PowerMeterMinigame::keyPressed() {
    if (_kbhit()) {
        _getch(); //eat the keypress so it doesn't show up
        return true;
    }
    return false;
}
#endif // _WIN32
PowerMeterMinigame::PowerMeterMinigame() {
    // set up random numbers and the meter size
    srand(time(NULL)); 
    meterWidth = 50;
    gameSpeedMs = 60;
    targetZoneWidth = 6;
    // find a random spot for the target zone
    targetZoneStart = 3 + (rand() % (meterWidth - targetZoneWidth - 6)); 
    targetZoneEnd = targetZoneStart + targetZoneWidth;
    cursorPosition = 0;
    direction = 1;
}

bool PowerMeterMinigame::run() {
    cout << "\n\n  [status] starting auxiliary power calibration...\n";
    
#ifndef _WIN32
    // set up the terminal for the game loop (mac/linux)
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    bool stopped = false;
    while (!stopped) {
        if (keyPressed()) {
            stopped = true; 
        }
        
        if (!stopped) {
            update();
            display();
            // wait for next frame
            this_thread::sleep_for(milliseconds(gameSpeedMs));
        }
    }
#ifndef _WIN32
    // put the terminal back to normal after the loop
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    bool win = checkWinCondition(cursorPosition);
    // clear the screen for the end message
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    cout << "\n------------------------------------------------\n";
    cout << "  auxiliary power control (stopped at: " << cursorPosition << ")\n";
    cout << "  ";
    // draw the meter one last time
    for (int i = 0; i < meterWidth; ++i)
        cout << ((i >= targetZoneStart && i < targetZoneEnd) ? "=" : "-");
    cout << "\n  ";
    for (int i = 0; i < cursorPosition; ++i) cout << " ";
    cout << "^\n";
    
    cout << "\n  >> " << (win ? "success! power is back online." 
                              : "failure: a power surge was detected.") << " <<\n";
    cout << "------------------------------------------------\n";
    
    // wait abit before continuing
    this_thread::sleep_for(seconds(2));
    
    return win;
}

void PowerMeterMinigame::update() {
    cursorPosition += direction;
    //change direction if the cursor hits the edge
    if (cursorPosition <= 0 || cursorPosition >= meterWidth)
        direction *= -1;
}

void PowerMeterMinigame::display() {
    // clear screen for animation so it looks smooth
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    cout << "\n\n";
    cout << "      [=== auxiliary power control ===]\n\n";
    cout << "  goal: stop the cursor (^) inside the '=' zone! press any key!\n\n";

    cout << "  [";
    // draw the meter bar
    for (int i = 0; i < meterWidth; ++i)
        cout << ((i >= targetZoneStart && i < targetZoneEnd) ? "=" : "-");
    cout << "]\n  ";

    //draw the cursor (^)
    for (int i = 0; i < cursorPosition; ++i) cout << " ";
    cout << "^\n\n  [status] press any key to stop the cursor.\n";
}

bool PowerMeterMinigame::checkWinCondition(int finalPosition) {
    // check if the cursor stopped in the target area
    return finalPosition >= targetZoneStart && finalPosition < targetZoneEnd;
}
