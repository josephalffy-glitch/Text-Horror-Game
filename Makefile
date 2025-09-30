final:
	g++ player.cpp main.cpp -o Game.exe

run:
	Game.exe

clean:
	rm -f *.o *.exe Game.exe *.gch || del /Q *.o *.exe *.gch

.PHONY: final run clean
