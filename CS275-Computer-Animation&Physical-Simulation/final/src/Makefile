TARGET = main
CXX = g++
CXXFLAGS = -O2 -std=c++14
LIBS = -lGL -lGLU -lGLEW -lglfw

all: main
main: sph.cpp hash.cpp cube.cpp main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

.PHONY clean:
	rm -f ./*.d 
	rm -f ./main