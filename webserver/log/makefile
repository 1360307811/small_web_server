cc=g++

all :fileutil.o logfile.o main

main: logfile.o	fileutil.o
	$(cc) -o main logfile.o fileutil.o 
logfile.o: logfile.cpp 
	$(cc) -c logfile.cpp  -std=c++11    
fileutil.o:fileutil.cpp fileutil.h
	$(cc) -c fileutil.cpp  -std=c++11  

.PHONY: clean test
test:
	./main

clean:
	rm -f *.o main 

