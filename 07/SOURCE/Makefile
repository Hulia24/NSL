CC = g++-15

INCLUDES = -I/usr/local/opt/armadillo/include
LIBS = -L/usr/local/opt/armadillo/lib

CFLAGS = -O3 --std=c++14 $(INCLUDES)
AFLAGS = $(LIBS) -larmadillo

all: simulator.exe temperatura.exe acceptance.exe calibration.exe simulation.exe

simulator.exe : system.o NSL_SIMULATOR.o particle.o random.o
	$(CC) system.o particle.o random.o NSL_SIMULATOR.o -o simulator.exe $(AFLAGS)
calibration.exe : system.o CALIBRATION.o particle.o random.o
	$(CC) system.o particle.o random.o CALIBRATION.o -o calibration.exe $(AFLAGS)
simulation.exe : system.o SIMULATION.o particle.o random.o
	$(CC) system.o particle.o random.o SIMULATION.o -o simulation.exe $(AFLAGS)

NSL_SIMULATOR.o : NSL_SIMULATOR.cpp system.h particle.h
	$(CC) -c NSL_SIMULATOR.cpp -o NSL_SIMULATOR.o $(CFLAGS)
SIMULATION.o : simulation.cpp utils.h system.h particle.h
	$(CC) -c simulation.cpp -o SIMULATION.o $(CFLAGS)
CALIBRATION.o : calibration.cpp utils.h system.h particle.h
	$(CC) -c calibration.cpp -o CALIBRATION.o $(CFLAGS)
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
