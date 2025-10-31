final:
	g++ player.cpp Room.cpp main.cpp GameEngine.cpp -o Game.exe

run:
	g++ player.cpp Room.cpp main.cpp GameEngine.cpp -o Game.exe
	./Game.exe

clean:
	rm -f *.o *.exe Game.exe *.gch || del /Q *.o *.exe *.gch

.PHONY: final run clean
