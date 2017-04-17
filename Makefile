CXX=x86_64-w64-mingw32-g++
CXF=-municode -Wno-write-strings

INC=-I./SDL2-2.0.5/include -I./freetype2/include
LIB=-L./SDL2-2.0.5-bin/Debug -L./freetype2-bin/Debug
LNK=-lSDL2 -lSDL2main -lfreetyped

SRC=$(wildcard *.cc)
OBJ=${SRC:.cc=.o}
BIN=console.exe

.phony: clean

${BIN}: ${OBJ}	
	${CXX} ${LIB} ${LNK} -o $@ $^

%.o: %.cc
	${CXX} ${CXF} ${INC} -c -o $@ $<

clean:
	rm ${OBJ} ${BIN}