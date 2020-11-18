TARGET     = main
DEFINES   += -DNDEBUG
INCLUDES  += -I. -I/usr/local/include
ALL_CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES) -std=c++11
ALL_CFLAGS    += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -O0 -Wall -Wextra -Wno-deprecated-declarations
ALL_CXXFLAGS  += $(CXXFLAGS) $(ALL_CFLAGS)
ALL_RESFLAGS  += $(RESFLAGS) $(DEFINES) $(INCLUDES)
ALL_LDFLAGS   += $(LDFLAGS) -L/usr/local/lib -L. -Wl,-x -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework GLUT
LIBS      += $(LDDEPS) -lglfw -lGLEW
CXX		   = clang++

all: cube main 
cube: cube.hpp cube.cpp
	$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) $(LIBS) cube.cpp -shared -o cube.o

main: rigidBody.hpp rigidBody.ipp basicTypes.hpp main.cpp
	$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) $(LIBS) cube.o main.cpp  -o main

.PHONY clean:
	rm -f ./main ./main.d ./main.o
	rm -f ./cube.d ./cube.o
	rm -f ./rigidBody.d ./rigidBody.o


