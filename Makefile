final:
	g++ player.cpp main.cpp GameEngine.cpp -o Game.exe

run:
	g++ player.cpp main.cpp GameEngine.cpp -o Game.exe
	./Game.exe

clean:
	rm -f *.o *.exe Game.exe *.gch || del /Q *.o *.exe *.gch

.PHONY: final run clean
