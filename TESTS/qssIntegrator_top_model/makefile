CC=g++
CFLAGS=-std=c++17
COMPILE_TARGET=NUCLEO_F401RE
FLASH_TARGET=NODE_F401RE
EXECUTABLE_NAME=QSS_INTEGRATOR_TOP

INCLUDRT_ARM_MBED=-I ../../../cadmium/include
INCLUDEDESTIMES=-I ../../../cadmium/DESTimes/include

all: qssIntegrator_top.o 
	$(CC) -g -o $(EXECUTABLE_NAME) qssIntegrator_top.o 

qssIntegrator_top.o: qssIntegrator_top.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDRT_ARM_MBED) $(INCLUDEDESTIMES) qssIntegrator_top.cpp -o qssIntegrator_top.o

clean:
	rm -f $(EXECUTABLE_NAME) *.o *~
