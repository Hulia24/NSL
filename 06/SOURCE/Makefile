CC = g++
CFLAGS = -O3 --std=c++14
AFLAGS = -larmadillo

all: simulator.exe scan.exe

simulator.exe : system.o NSL_SIMULATOR.o particle.o random.o
	$(CC) system.o particle.o random.o NSL_SIMULATOR.o -o simulator.exe $(AFLAGS)
scan.exe : system.o TOTAL_SIM.o particle.o random.o
	$(CC) system.o particle.o random.o TOTAL_SIM.o -o scan.exe $(AFLAGS)

NSL_SIMULATOR.o : NSL_SIMULATOR.cpp system.h particle.h
	$(CC) -c NSL_SIMULATOR.cpp -o NSL_SIMULATOR.o $(CFLAGS)
TOTAL_SIM.o : TOTAL_SIM.cpp system.h particle.h
	$(CC) -c TOTAL_SIM.cpp -o TOTAL_SIM.o $(CFLAGS)
system.o : system.cpp system.h random.h particle.h
	$(CC) -c system.cpp -o system.o $(CFLAGS)
particle.o : particle.cpp particle.h
	$(CC) -c particle.cpp -o particle.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o simulator.exe
remove:
	rm ../OUTPUT/*.*
	rm ../OUTPUT/CONFIG/*.*