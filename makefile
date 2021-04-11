all: main GenPlayer

main: main.cpp
	g++ main.cpp -o main

GenPlayer: GenPlayer.cpp
	g++ GenPlayer.cpp -o GenPlayer

run: main
	./main

clean:
	rm main
