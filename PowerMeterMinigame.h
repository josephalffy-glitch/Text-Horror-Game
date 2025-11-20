#ifndef POWER_METER_MINIGAME_H
#define POWER_METER_MINIGAME_H
// we need chrono and cstdlib here for the constructor
#include <cstdlib>
#include <chrono>

class PowerMeterMinigame {
public:
    int meterWidth;
    int targetZoneStart;
    int targetZoneEnd;
    int targetZoneWidth;

    int cursorPosition;
    int direction;  // 1 means move right, -1 means move left
    int gameSpeedMs;

    void update();  // changes the cursor's position each frame
    void display(); // draws the game meter to the console
    bool checkWinCondition(int finalPosition); // checks if the player stopped in the target zone
    bool keyPressed(); // checks if the user pressed any key

public:
    
    PowerMeterMinigame(); // the setup function for the minigame
    bool run(); // starts the main game loop
};

#endif
