CPPFLAG=--std=c++11

all: main GenPlayer Team

main: main.o Player.o Team.o
	g++ main.o Player.o Team.o -o main

GenPlayer: GenPlayer.cpp
	g++ GenPlayer.cpp -o GenPlayer

run: main
	./main

diff: main
	./main | tee out.txt
	diff out.txt in.txt

clean:
	@rm main
	@rm GenPlayer
	@rm Player
	@rm *.o

main.o: main.cpp
	g++ -c main.cpp ${CPPFLAG}

Player.o: Player.cpp Player.hpp
	g++ -c Player.cpp ${CPPFLAG}

Team.o: Team.cpp Team.hpp
	g++ -c Team.cpp ${CPPFLAG}

Team: Team.cpp Team.hpp
	g++ Team.cpp ${CPPFLAG} -D__UT_PLAYER__ -o Team
