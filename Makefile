SHELL = /bin/sh
SRC = src
OBJS =  ${SRC}/main.o ${SRC}/Helpers.o ${SRC}/Minkowski.o
CXXFLAGS = -std=c++11
CXX = g++
LIBS =  -lsfml-graphics -lsfml-window -lsfml-system

minkowski:${OBJS}
	${CXX} ${CXXFLAGS} -o $@ ${OBJS} ${LIBS}

clean:
	-rm -f ${SRC}/*.o minkowski
