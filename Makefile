CXX=g++
CXXFLAGS=-std=c++11 -Wall -g

main: test.o abstractJWriter.o ASCIIJWriter.o jState.o
	$(CXX) $(CXXFLAGS) -o main *.o

test.o: ASCIIJWriter.h   

abstractJWriter.o: abstractJWriter.h abstractJWriter.cpp jDefines.h jState.h

ASCIIJWriter.o: ASCIIJWriter.h ASCIIJWriter.cpp abstractJWriter.h jDefines.h 

jState.o: jState.h jState.cpp jDefines.h

clean:
	rm *.o main
