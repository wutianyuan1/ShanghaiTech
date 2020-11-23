TARGET     		= main
GRID	  		?= off
INCLUDES  		+= -I. -I/usr/local/include
ALL_CPPFLAGS  	+= -MMD -MP $(DEFINES) $(INCLUDES) -std=c++14
ALL_CFLAGS    	+= $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -O2 -Wall -Wextra -Wno-deprecated-declarations
ALL_CXXFLAGS  	+= $(CXXFLAGS) $(ALL_CFLAGS)
ALL_RESFLAGS  	+= $(RESFLAGS) $(DEFINES) $(INCLUDES)
ALL_LDFLAGS   	+= $(LDFLAGS) -L/usr/local/lib -L. -Wl,-x -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
LIBS      		+= $(LDDEPS) -lglfw -lGLEW
CXX		   		= clang++

ifeq ($(GRID), on)
	DEFINES = -DSHOW_MESHGRID
endif

all: mesh solver main

mesh: mesh.hpp mesh.cpp
	$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) $(LIBS) $(DEFINES) mesh.cpp -shared -o mesh.o

solver: solver.hpp solver.cpp
	$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) $(LIBS) $(DEFINES) solver.cpp -shared -o solver.o

main: mesh.o solver.o main.cpp
	$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) $(LIBS) main.cpp solver.o mesh.o -o main

.PHONY clean:
	rm -f ./main ./main.d ./main.o
	rm -f ./mesh.d ./mesh.o
	rm -f ./solver.d ./solver.o


