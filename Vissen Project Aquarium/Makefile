CFLAGS=-g3
CXXFLAGS=-g3
LIBS=`pkg-config --libs libglfw` -lGLU -ljpeg

all: VissenProject

VissenProject: AquariumController.o Bubble.o Environment.o getline.o Ground.o imagereceiver.o JPEG.o main.o Matrix.o MS3D_ASCII.o Object.o safe_getline.o Vector.o Vis.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f VissenProject *.o

.PHONY: all clean
