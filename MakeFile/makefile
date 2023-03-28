output: main.o Test.o
	g++ main.o Test.o -o myexe 

main.o: main.cpp
	g++ -c main.cpp

Test.o: Test.cpp Test.h
	g++ -c Test.cpp
	
clean: 
	rm *.o output
