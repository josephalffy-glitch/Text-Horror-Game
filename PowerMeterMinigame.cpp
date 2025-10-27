#include "PowerMeterMinigame.h"
#include "GameEngine.h"
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>
#include <ctime>

using namespace std;

#ifdef _WIN32
#include <conio.h>
#else
// this is only for mac since i work on mac
#include <unistd.h>
#include <termios.h>
#endif

PowerMeterMinigame::PowerMeterMinigame() {
    srand(time(NULL));
    meterWidth = 50;
    gameSpeedMs = 60;
    targetZoneWidth = 6;
    targetZoneStart = 3 + (rand() % (meterWidth - targetZoneWidth - 6));
    targetZoneEnd = targetZoneStart + targetZoneWidth;
    cursorPosition = 0;
    direction = 1;
}

bool PowerMeterMinigame::run() {
    while (true) {
        if (keyPressed()) break; // stop when any key is pressed
        update();
        display();
        this_thread::sleep_for(chrono::milliseconds(gameSpeedMs));
    }

    bool win = checkWinCondition(cursorPosition);
    cout << "\n\n  >> " << (win ? "nice! power is back online." 
                              : "ah, missed it. a power surge was detected.") << " <<\n";
    this_thread::sleep_for(chrono::seconds(2));
    return win;
}

void PowerMeterMinigame::update() {
    cursorPosition += direction;
    if (cursorPosition <= 0 || cursorPosition >= meterWidth)
        direction *= -1;
}

void PowerMeterMinigame::display() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    cout << "\n\n";
    cout << "      [=== AUXILIARY POWER CONTROL ===]\n\n";
    cout << "  Goal: Stop the cursor (^) inside the '=' zone!\n\n";

    cout << "  [";
    for (int i = 0; i < meterWidth; ++i)
        cout << ((i >= targetZoneStart && i < targetZoneEnd) ? "=" : "-");
    cout << "]\n  ";

    for (int i = 0; i <= cursorPosition; ++i) cout << " ";
    cout << "^\n\n  [status] press any key to stop the cursor.\n";
}

bool PowerMeterMinigame::checkWinCondition(int finalPosition) {
    return finalPosition >= targetZoneStart && finalPosition < targetZoneEnd;
}

bool PowerMeterMinigame::keyPressed() {
#ifdef _WIN32
    if (_kbhit()) {
        _getch();
        return true;
    }
    return false;
#else
    // this is only for mac since i work on mac
    struct termios oldt, newt;
    int bytesRead;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    bytesRead = read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return bytesRead > 0;
#endif
}