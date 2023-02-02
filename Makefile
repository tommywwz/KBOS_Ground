all: Taking_Flight Utils.o Taking_Flight.o

Taking_Flight: Utils.o Taking_Flight.o
	gcc -pthread -g -o Taking_Flight Utils.o Taking_Flight.o

Utils.o: Utils.c Utils.h
	gcc -pthread -c Utils.c

Taking_Flight.o: Taking_Flight.c
	gcc -pthread -c Taking_Flight.c

clean:
	rm -f *.o Taking_Flight
