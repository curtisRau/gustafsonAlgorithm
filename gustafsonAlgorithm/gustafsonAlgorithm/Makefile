
CXX = clang++

CXX_FLAGS = -g -std=c++11

main: main.cpp gustafsonAlgorithm.o besselFunctions.o
	$(CXX) main.cpp -o main.out $(CXX_FLAGS) gustafsonAlgorithm.o besselFunctions.o

gustafsonAlgorithm.o: gustafsonAlgorithm.cpp gustafsonAlgorithm.hpp
	$(CXX) gustafsonAlgorithm.cpp -o gustafsonAlgorithm.o -c $(CXX_FLAGS)

besselFunctions.o: besselFunctions.cpp besselFunctions.hpp
	$(CXX) besselFunctions.cpp -o besselFunctions.o -c $(CXX_FLAGS)

clean:
	rm -f *.o main.out
