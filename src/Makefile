CFLAGS = -Wall -g

cpmRun: diskSimulator.o  cpmfsys.o fsysdriver.o 
	gcc -o cpmRun diskSimulator.o cpmfsys.o fsysdriver.o $(CFLAGS)

diskSimulator.o: diskSimulator.c diskSimulator.h
	gcc -c diskSimulator.c $(CFLAGS)

cpmfsys.o: cpmfsys.h cpmfsys.c 
	gcc -c cpmfsys.c $(CFLAGS)

fsysdriver.o: fsysdriver.c
	gcc -c fsysdriver.c $(CFLAGS)

all: 
	cpmRun

clean: 
	rm *.o 

