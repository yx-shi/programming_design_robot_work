SOURCE=game.cpp actuator.cpp level.cpp robot.cpp
OBJECTS=$(SOURCE:.cpp=.o)

all: $(SOURCE)
	 g++ -o game $(SOURCE) -std=c++11

level: level.cpp
	 g++ -c level.cpp -std=c++11 -o level.o

robot: robot.cpp
	 g++ -c robot.cpp -std=c++11 -o robot.o
actuator: actuator.cpp
	 g++ -c actuator.cpp -std=c++11 -o actuator.o

.PHONY: clean
clean:
	 rm -f game

