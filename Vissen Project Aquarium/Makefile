CFLAGS=-g3 -Wall -Wextra
CXXFLAGS=-g3 -Wall -Wextra
CPPFLAGS=-I/usr/include/eigen2
LDFLAGS=

# Enable profiling
CFLAGS+=-pg
CXXFLAGS+=-pg
LDFLAGS+=-pg

LIBS=`pkg-config --libs libglfw` -lGLU -ljpeg

all: VissenProject

VissenProject: AquariumController.o Bubble.o Environment.o GL/GLee.o Ground.o imagereceiver.o JPEG.o main.o MS3D_ASCII.o Object.o Vis.o wiggle.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f VissenProject *.o

.PHONY: all clean
