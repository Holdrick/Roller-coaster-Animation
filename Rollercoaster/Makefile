EXEFILE = asg1
LIBS = -lglfw -lGLEW -lGL
LIBDIRS += -L/usr/X11R6/lib -L/usr/X11R6/lib64 -L/usr/local/lib
INCDIRS += -I/usr/include -I/usr/local/include -I/usr/include/GL


$(EXEFILE): Main.o Track.o GOBJ.o
	g++ -o $(EXEFILE) $(LIBDIRS) $(INCDIRS) $(LIBS) Main.o Track.o GOBJ.o

Main.o: Main.cpp Track.h GOBJ.h
	g++ -c Main.cpp

Track.o: Track.cpp Track.h
	g++ -c Track.cpp

GOBJ.o: GOBJ.cpp GOBJ.h
	g++ -c GOBJ.cpp

clean:
	rm $(EXEFILE) GOBJ.o Track.o Main.o
