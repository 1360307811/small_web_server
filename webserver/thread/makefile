cc=g++
OBJS=Threadtest.o CountDownLatch.o Thread.o 
all:Threadtest.o CountDownLatch.o Thread.o  main
CFLAGS:-g -Wall 

main:$(OBJS)
	g++  $(CFLAGS) -o main $(OBJS) -lpthread 
Threadtest.o: Threadtest.cpp Thread.h
	$(cc) -c Threadtest.cpp -std=c++11    

Thread.o: Thread.cpp Thread.h CountDownLatch.h
	$(cc) -c Thread.cpp  -std=c++11    
CountDownLatch.o: CountDownLatch.cpp CountDownLatch.h
	$(cc) -c CountDownLatch.cpp  -std=c++11  

.PHONY: clean test
test:
	./main

clean:
	rm -f *.o main 

