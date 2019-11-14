# Example SDL21
EXE=sdl21

# Main target
all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
errcheck.o: errcheck.c CSCIx229.h
fatal.o: fatal.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
print-dl.o: print-dl.c CSCIx229.h
project.o: project.c CSCIx229.h
sdl21.o: sdl21.c CSCIx229.h

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
sdl21:sdl21.o fatal.o loadtexbmp.o project.o errcheck.o print-dl.o
	gcc -O3 -o $@ $^  -lSDL -lSDL_mixer -lGLU -lGL -lm

#  Clean
clean:
	$(CLEAN)
