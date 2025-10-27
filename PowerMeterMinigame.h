#ifndef POWER_METER_MINIGAME_H
#define POWER_METER_MINIGAME_H

class PowerMeterMinigame {
private:
    int meterWidth;
    int targetZoneStart;
    int targetZoneEnd;
    int targetZoneWidth;

    int cursorPosition;
    int direction;      // 1 = right, -1 = left
    int gameSpeedMs;

    void update();      // moves the cursor
    void display();     // draws the game
    bool checkWinCondition(int finalPosition);
    bool keyPressed();  

public:
    PowerMeterMinigame(); // constructor to set things up
    bool run();            // the main function to start the game
};

#endif
