CC=g++
CFLAGS=-std=c++17
COMPILE_TARGET=NUCLEO_F401RE
FLASH_TARGET=NODE_F401RE
EXECUTABLE_NAME=PID_TOP

INCLUDRT_ARM_MBED=-I ../../../cadmium/include
INCLUDEDESTIMES=-I ../../../cadmium/DESTimes/include

all: pid.o 
	$(CC) -g -o $(EXECUTABLE_NAME) pid.o 

pid.o: pid.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDRT_ARM_MBED) $(INCLUDEDESTIMES) pid.cpp -o pid.o

clean:
	rm -f $(EXECUTABLE_NAME) *.o *~
