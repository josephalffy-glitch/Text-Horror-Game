final:
	g++ player.cpp Room.cpp main.cpp GameEngine.cpp PowerMeterMinigame.cpp -o Game.exe -pthread

run:
	g++ player.cpp Room.cpp main.cpp GameEngine.cpp PowerMeterMinigame.cpp -o Game.exe -pthread
	./Game.exe

clean:
	rm -f *.o *.exe Game.exe *.gch || del /Q *.o *.exe *.gch

.PHONY: final run clean
