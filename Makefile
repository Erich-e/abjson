FLAGS=-std=c++11 -Wall -g
CPP=g++

main:	test.o jWriter.h jDefines.h jData.o
	$(CPP) $(FLAGS) -o main test.o jData.o

test.o:	test.cpp jWriter.h
	$(CPP) $(FLAGS) -c test.cpp

jData.o:	jData.cpp
	$(CPP) $(FLAGS) -c jData.cpp

clean:
	rm *.o main
